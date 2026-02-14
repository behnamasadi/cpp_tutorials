# XML SOAP (Simple Object Access Protocol) with C++

## What is SOAP?

SOAP (Simple Object Access Protocol) is a messaging protocol specification for exchanging structured information in web services. Unlike REST, which is an architectural style, SOAP is a strict protocol with:

- **XML-based message format** -- all requests and responses are XML documents
- **Transport independence** -- typically uses HTTP/HTTPS, but can also use SMTP, TCP, etc.
- **Built-in error handling** -- standardized `<Fault>` element for error reporting
- **WSDL** (Web Services Description Language) -- a formal contract describing the service

## SOAP vs REST

| Feature          | SOAP                          | REST                         |
| ---------------- | ----------------------------- | ---------------------------- |
| Format           | XML only                      | JSON, XML, plain text, etc.  |
| Protocol         | Strict protocol (W3C)         | Architectural style          |
| Transport        | HTTP, SMTP, TCP, etc.         | Primarily HTTP               |
| Contract         | WSDL                          | OpenAPI / Swagger (optional) |
| Statefulness     | Can be stateful or stateless  | Stateless                    |
| Error handling   | `<soap:Fault>` element        | HTTP status codes            |
| Security         | WS-Security (built-in)        | TLS + OAuth / API keys       |
| Use cases        | Enterprise, banking, legacy   | Web/mobile APIs, microservices |

## SOAP Message Structure

A SOAP message is an XML document with the following structure:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<soap:Envelope xmlns:soap="http://www.w3.org/2003/05/soap-envelope"
               xmlns:ws="http://example.com/webservice">
    <soap:Header>
        <!-- Optional: authentication, routing, etc. -->
    </soap:Header>
    <soap:Body>
        <!-- The actual request/response payload -->
        <ws:GetWeather>
            <ws:City>Berlin</ws:City>
        </ws:GetWeather>
    </soap:Body>
</soap:Envelope>
```

## SOAP with C++ (tinyxml2 + cURL)

Since this project already uses **tinyxml2** for XML parsing and **cURL** for HTTP communication, we can combine both to build a SOAP client without any additional dependencies.

### Building the SOAP Request XML

```cpp
tinyxml2::XMLDocument doc;

auto* decl = doc.NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
doc.InsertFirstChild(decl);

auto* envelope = doc.NewElement("soap:Envelope");
envelope->SetAttribute("xmlns:soap", "http://www.w3.org/2003/05/soap-envelope");
envelope->SetAttribute("xmlns:ws", "http://example.com/webservice");
doc.InsertEndChild(envelope);

auto* body = doc.NewElement("soap:Body");
envelope->InsertEndChild(body);

auto* method = doc.NewElement("ws:GetWeather");
body->InsertEndChild(method);

auto* param = doc.NewElement("ws:City");
param->SetText("Berlin");
method->InsertEndChild(param);
```

### Sending the SOAP Request with cURL

```cpp
curl_easy_setopt(curl, CURLOPT_URL, "http://example.com/soap-endpoint");
curl_easy_setopt(curl, CURLOPT_POST, 1L);
curl_easy_setopt(curl, CURLOPT_POSTFIELDS, xmlString.c_str());

struct curl_slist* headers = nullptr;
headers = curl_slist_append(headers, "Content-Type: application/soap+xml; charset=utf-8");
headers = curl_slist_append(headers, "SOAPAction: \"http://example.com/GetWeather\"");
curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
```

### Parsing the SOAP Response

```cpp
tinyxml2::XMLDocument responseDoc;
responseDoc.Parse(responseString.c_str());

auto* respEnvelope = responseDoc.FirstChildElement("soap:Envelope");
auto* respBody = respEnvelope->FirstChildElement("soap:Body");
// Navigate to the result element...
```

## SOAP Fault Handling

When a SOAP service encounters an error, it returns a `<soap:Fault>` element:

```xml
<soap:Body>
    <soap:Fault>
        <soap:Code>
            <soap:Value>soap:Sender</soap:Value>
        </soap:Code>
        <soap:Reason>
            <soap:Text xml:lang="en">Invalid city name</soap:Text>
        </soap:Reason>
    </soap:Fault>
</soap:Body>
```

In C++, you should always check for faults before processing the response:

```cpp
auto* fault = respBody->FirstChildElement("soap:Fault");
if (fault) {
    auto* reason = fault->FirstChildElement("soap:Reason");
    auto* text = reason ? reason->FirstChildElement("soap:Text") : nullptr;
    std::cerr << "SOAP Fault: " << (text ? text->GetText() : "Unknown error") << std::endl;
}
```

## When to Use SOAP

- Integrating with **enterprise systems** (banking, ERP, CRM)
- **Legacy web services** that only expose SOAP endpoints
- When you need **WS-Security** for message-level encryption/signing
- Formal contracts via **WSDL** are required

## Dedicated SOAP Libraries for C++

For larger SOAP projects, consider these dedicated libraries:

- **[gSOAP](https://www.genivia.com/dev.html)** -- the most mature C/C++ SOAP toolkit; auto-generates stubs from WSDL; dual-licensed (GPLv2 / commercial)
- **[Apache Axis2/C](https://axis.apache.org/axis2/c/core/)** -- C implementation of the SOAP stack (less actively maintained)

## Build

Enable with CMake:

```bash
cmake -G "Ninja Multi-Config" -S . -B build -DENABLE_SOAP=ON -DENABLE_XML=ON -DENABLE_CURL=ON
cmake --build build --config Release --target xml_soap_example
```

The `ENABLE_SOAP` option requires both `ENABLE_XML` (tinyxml2) and `ENABLE_CURL` (libcurl) to be ON.
