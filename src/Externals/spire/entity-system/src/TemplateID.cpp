/// \author James Hughes
/// \date   December 2013

#include "TemplateID.hpp"

namespace spire {

uint64_t TemplateIDHelper::mCurrentTypeID = TemplateIDHelper::InitialStartingID;


std::map<std::string, uint64_t> TemplateID::ids_;

} // namespace spire

