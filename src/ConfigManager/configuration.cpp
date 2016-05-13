#include "configuration.h"

namespace ConfigManager
{
  Configuration::Configuration()
  {
  }
  
  Configuration::Configuration(std::string filename, InputFilePolicy policy = InputFilePolicy::NONE)
  {
  }
  
  Configuration::~Configuration()
  {
  }
  
  void Configuration::SetConfigFile(std::string filename, InputFilePolicy policy = InputFilePolicy::NONE)
  {
  }

  void Configuration::SetInputFile(std::string filename)
  {
  }
  
  void Configuration::SetInputStream(std::istream& input_stream)
  {
  }

  void Configuration::CheckStrict()
  {
  }
  
  void Configuration::SetOutputFile(std::string filename)
  {
  }
  
  void Configuration::SetOutputStream(std::ostream& output_stream)
  {
  }
  
  void Configuration::Flush(OutputMethod output_method = OutputMethod::NORMAL)
  {
  }
  
  Section Configuration::SpecifySection(std::string section_name, Requirement requirement = Requirement::OPTIONAL, const std::string comments = "")
  {
    return Section(section_name);
  }
};
