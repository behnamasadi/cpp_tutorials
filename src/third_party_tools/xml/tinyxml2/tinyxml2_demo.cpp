#include <iostream>
#include <tinyxml2.h>

void generatingXMLDocumentS() {
  /////////////////////////// XML document from string
  //////////////////////////////
  std::string xmlString = R"(
	<information>
		<attributeApproach value='10' />
		<textApproach>
			<index>2</index>
		</textApproach>
	</information>)";
  tinyxml2::XMLDocument docFromString;
  if (docFromString.Parse(xmlString.c_str()) == tinyxml2::XML_SUCCESS) {
    docFromString.SaveFile("information.xml");
  }

  /////////////////////////// XML document from xml file on the disk
  //////////////////////////////
  tinyxml2::XMLDocument docFromFile;
  docFromFile.LoadFile("information.xml");
  int value, index;
  docFromFile.FirstChildElement()->FirstChildElement()->QueryIntAttribute(
      "value", &value);
  std::cout << "value: " << value << std::endl;
  docFromFile.FirstChildElement()
      ->FirstChildElement("textApproach")
      ->FirstChildElement("index")
      ->QueryIntText(&index);
  std::cout << "index: " << index << std::endl;

  /////////////////////////// dynamically create XML doc
  //////////////////////////////
  /*
  <information>
          <attributeApproach value="10"/>
          <textApproach>
                  <index>2</index>
          </textApproach>
  </information>

  we would like to change this to:

  <information>
          <attributeApproach value="10"/>
          <textApproach>
                  <index>2</index>
                  <!--Selected Player-->
                  <path name="selected_player">New Text</path>
          </textApproach>
  </information>
  */

  tinyxml2::XMLElement *textApproach =
      docFromFile.FirstChildElement()->FirstChildElement("textApproach");
  tinyxml2::XMLElement *indexNode = textApproach->FirstChildElement();

  // write the text
  tinyxml2::XMLComment *newComment = docFromFile.NewComment("Selected Player");
  tinyxml2::XMLElement *newElement = docFromFile.NewElement("path");

  newElement->SetAttribute("name", "selected_player");
  newElement->SetText("some text here");

  textApproach->InsertAfterChild(indexNode, newComment);
  textApproach->InsertAfterChild(newComment, newElement);

  tinyxml2::XMLDocument dynamicallyCreatedDoc;
  tinyxml2::XMLNode *node = dynamicallyCreatedDoc.NewElement("root");
  docFromFile.SaveFile("dynamically_created_doc.xml");
}

int main(int argc, char **argv) { generatingXMLDocumentS(); }
