#pragma once

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include "logging/Logger.hpp"
#include "xml/XMLTag.hpp"
#include <libxml/parser.h>
#include <libxml/SAX2.h>

namespace precice {
namespace logging {
class Logger;
} // namespace logging

namespace xml {
class XMLTag; // forward declaration to resolve circular import
struct ConfigurationContext;

/// Decodes escape sequences of a given xml
std::string decodeXML(std::string_view xml);

class ConfigParser {
public:
  /// Struct holding the read tag from xml file
  struct CTag {
    std::string m_Name;
    std::string m_Prefix;
    bool        m_Used = false;

    using AttributePair = std::map<std::string, std::string>;
    AttributePair                      m_aAttributes;
    std::vector<std::shared_ptr<CTag>> m_aSubTags;
  };

  using CTagPtrVec = std::vector<std::shared_ptr<CTag>>;

private:
  static precice::logging::Logger _log;

  /// the hash of the last processed config
  std::string _hash;

  CTagPtrVec m_AllTags;
  CTagPtrVec m_CurrentTags;

  std::shared_ptr<precice::xml::XMLTag> m_pXmlTag;

  /// Parser context — valid only during xmlParseChunk, null otherwise
  xmlParserCtxtPtr _parserContext = nullptr;

  /// Full file content saved for snippet extraction
  std::string _fileContent;

  /// File path for display in error messages
  std::string _filePath;

public:
  /// Parser ctor for Callback init
  ConfigParser(std::string_view filePath, const ConfigurationContext &context, std::shared_ptr<XMLTag> pXmlTag);

  /// Parser ctor without Callbacks
  ConfigParser(std::string_view filePath);

  /// Reads the xml file
  int readXmlFile(std::string const &filePath);

  /// returns the hash of the processed XML file
  std::string hash() const;

  /**
   * @brief Connects the actual tags of an xml layer with the predefined tags
   * @param DefTags predefined tags
   * @param SubTags actual tags from xml file
   */
  void connectTags(const ConfigurationContext &context, std::vector<std::shared_ptr<precice::xml::XMLTag>> &DefTags, CTagPtrVec &SubTags);

  /// Callback for Start-Tag
  void OnStartElement(
      std::string_view    localname,
      std::string_view    prefix,
      CTag::AttributePair attributes);

  /// Callback for End-Tag
  void OnEndElement();

  /// Callback for text sections in xml file
  void OnTextSection(const std::string &ch);

  /// Proxy for error and warning messages from libxml2
  static void MessageProxy(int level, std::string_view mess);

  /// Holds location info of the current XML tag being parsed
  struct XMLTagLocation {
    long        line    = -1;
    long        column  = -1;
    std::string snippet;
  };

  /// Returns the current line, column and source snippet from the parser
  XMLTagLocation getCurrentLocation() const;
};
} // namespace xml
} // namespace precice
