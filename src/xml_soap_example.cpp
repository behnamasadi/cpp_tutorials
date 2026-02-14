/*
 * XML SOAP Example using tinyxml2 + cURL
 *
 * Demonstrates how to:
 *   1. Build a SOAP XML request envelope with tinyxml2
 *   2. Send it via HTTP POST using cURL
 *   3. Parse the SOAP XML response
 *   4. Handle SOAP Faults
 *
 * This example targets a public SOAP service (number-to-words converter)
 * to show a real end-to-end SOAP call in C++.
 *
 * Build:
 *   cmake -G "Ninja Multi-Config" -S . -B build \
 *         -DENABLE_SOAP=ON -DENABLE_XML=ON -DENABLE_CURL=ON
 *   cmake --build build --config Release --target xml_soap_example
 */

#include <curl/curl.h>
#include <tinyxml2.h>

#include <iostream>
#include <sstream>
#include <string>

// ---------------------------------------------------------------------------
// cURL write callback -- appends received data to a std::string
// ---------------------------------------------------------------------------
static size_t writeCallback(void *contents, size_t size, size_t nmemb,
                            void *userp) {
  auto *response = static_cast<std::string *>(userp);
  response->append(static_cast<char *>(contents), size * nmemb);
  return size * nmemb;
}

// ---------------------------------------------------------------------------
// Helper: serialise tinyxml2 document to std::string
// ---------------------------------------------------------------------------
static std::string xmlToString(tinyxml2::XMLDocument &doc) {
  tinyxml2::XMLPrinter printer;
  doc.Print(&printer);
  return printer.CStr();
}

// ---------------------------------------------------------------------------
// Build SOAP envelope requesting NumberToWords conversion
//
// Target WSDL:
//   https://www.dataaccess.com/webservicesserver/NumberConversion.wso?WSDL
//
// SOAP 1.1 envelope (the service uses SOAP 1.1):
//
// <soap:Envelope xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/"
//                xmlns:web="http://www.dataaccess.com/webservicesserver/">
//   <soap:Body>
//     <web:NumberToWords>
//       <web:ubiNum>42</web:ubiNum>
//     </web:NumberToWords>
//   </soap:Body>
// </soap:Envelope>
// ---------------------------------------------------------------------------
static std::string buildSoapRequest(uint64_t number) {
  tinyxml2::XMLDocument doc;

  // XML declaration
  auto *decl = doc.NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
  doc.InsertFirstChild(decl);

  // <soap:Envelope>
  auto *envelope = doc.NewElement("soap:Envelope");
  envelope->SetAttribute("xmlns:soap",
                          "http://schemas.xmlsoap.org/soap/envelope/");
  envelope->SetAttribute("xmlns:web",
                          "http://www.dataaccess.com/webservicesserver/");
  doc.InsertEndChild(envelope);

  // <soap:Body>
  auto *body = doc.NewElement("soap:Body");
  envelope->InsertEndChild(body);

  // <web:NumberToWords>
  auto *method = doc.NewElement("web:NumberToWords");
  body->InsertEndChild(method);

  // <web:ubiNum>42</web:ubiNum>
  auto *param = doc.NewElement("web:ubiNum");
  param->SetText(number);
  method->InsertEndChild(param);

  return xmlToString(doc);
}

// ---------------------------------------------------------------------------
// Send the SOAP request via cURL and return the raw response body
// ---------------------------------------------------------------------------
static std::string sendSoapRequest(const std::string &url,
                                   const std::string &soapAction,
                                   const std::string &soapXml) {
  std::string response;

  CURL *curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Failed to initialise cURL" << std::endl;
    return {};
  }

  // HTTP headers required by SOAP 1.1
  struct curl_slist *headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: text/xml; charset=utf-8");
  std::string actionHeader = "SOAPAction: \"" + soapAction + "\"";
  headers = curl_slist_append(headers, actionHeader.c_str());

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, soapXml.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE,
                   static_cast<long>(soapXml.size()));
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
  }

  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);

  return response;
}

// ---------------------------------------------------------------------------
// Parse the SOAP response and extract the result text
// ---------------------------------------------------------------------------
static void parseSoapResponse(const std::string &responseXml) {
  if (responseXml.empty()) {
    std::cerr << "Empty response -- nothing to parse." << std::endl;
    return;
  }

  tinyxml2::XMLDocument doc;
  if (doc.Parse(responseXml.c_str()) != tinyxml2::XML_SUCCESS) {
    std::cerr << "Failed to parse response XML." << std::endl;
    return;
  }

  // Navigate: soap:Envelope -> soap:Body
  auto *envelope = doc.FirstChildElement("soap:Envelope");
  if (!envelope) {
    // Some servers use the full namespace prefix
    envelope = doc.FirstChildElement("SOAP-ENV:Envelope");
  }
  if (!envelope) {
    std::cerr << "Could not find SOAP Envelope in response." << std::endl;
    std::cout << "Raw response:\n" << responseXml << std::endl;
    return;
  }

  auto *body = envelope->FirstChildElement("soap:Body");
  if (!body) {
    body = envelope->FirstChildElement("SOAP-ENV:Body");
  }
  if (!body) {
    std::cerr << "Could not find SOAP Body in response." << std::endl;
    return;
  }

  // Check for SOAP Fault
  auto *fault = body->FirstChildElement("soap:Fault");
  if (!fault) {
    fault = body->FirstChildElement("SOAP-ENV:Fault");
  }
  if (fault) {
    auto *faultString = fault->FirstChildElement("faultstring");
    std::cerr << "SOAP Fault: "
              << (faultString ? faultString->GetText() : "Unknown error")
              << std::endl;
    return;
  }

  // Extract result: <m:NumberToWordsResponse> -> <m:NumberToWordsResult>
  // The namespace prefix may vary, so iterate children
  auto *resultContainer = body->FirstChildElement();
  if (resultContainer) {
    auto *resultElement = resultContainer->FirstChildElement();
    if (resultElement && resultElement->GetText()) {
      std::cout << "Result: " << resultElement->GetText() << std::endl;
    } else {
      std::cout << "No text in result element." << std::endl;
      std::cout << "Raw response:\n" << responseXml << std::endl;
    }
  } else {
    std::cout << "No child elements in SOAP Body." << std::endl;
    std::cout << "Raw response:\n" << responseXml << std::endl;
  }
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main() {
  std::cout << "=== XML SOAP Example (tinyxml2 + cURL) ===" << std::endl;

  // The number we want to convert to words
  uint64_t number = 42;

  // --- Step 1: Build the SOAP request ---
  std::string soapXml = buildSoapRequest(number);
  std::cout << "\n--- SOAP Request ---\n" << soapXml << std::endl;

  // --- Step 2: Send to the SOAP service ---
  const std::string url =
      "https://www.dataaccess.com/webservicesserver/NumberConversion.wso";
  const std::string soapAction =
      "http://www.dataaccess.com/webservicesserver/NumberToWords";

  std::cout << "Sending request to: " << url << std::endl;
  std::string responseXml = sendSoapRequest(url, soapAction, soapXml);
  std::cout << "\n--- SOAP Response ---\n" << responseXml << std::endl;

  // --- Step 3: Parse the response ---
  std::cout << "\n--- Parsed Result ---" << std::endl;
  parseSoapResponse(responseXml);

  return 0;
}
