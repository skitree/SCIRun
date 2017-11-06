/// \author James Hughes
/// \date   December 2013

#ifndef SPIRE_ENTITY_SYSTEM_TEMPLATEID_HPP
#define SPIRE_ENTITY_SYSTEM_TEMPLATEID_HPP

#include <es-log/trace-log.h>
#include <iostream>
#include <cstdint>
#include <spire/scishare.h>

namespace spire {

class SCISHARE TemplateIDHelper
{
public:

  static const int InitialStartingID = 0;

  static uint64_t getNewTypeID()
  {
    ++mCurrentTypeID;
    return mCurrentTypeID;
  }

  static void setCounter(uint64_t newCount) {mCurrentTypeID = newCount;}
  static uint64_t getCurrentTypeID()        {return mCurrentTypeID;}

private:
  static uint64_t mCurrentTypeID;
};

/// Simple templated class to extract a unique ID from types. Used mostly
/// for sorting purposes.

class SCISHARE TemplateID
{
public:
  template <typename T>
  static uint64_t getID()
  {
    auto it = ids_.find(T::getName());
    if (it == ids_.end())
    {
      auto mStaticTypeID = TemplateIDHelper::getNewTypeID();
      std::cout << "TemplateID " << T::getName() << " id: " << mStaticTypeID << std::endl;
      ids_[T::getName()] = mStaticTypeID;
      return mStaticTypeID;
    }

    return it->second;
  }

  //template <typename T>
  //static void setID(uint64_t id)
  //{
  //  mStaticTypeID = id;
  //}
private:
  static std::map<std::string, uint64_t> ids_;
};


} // namespace spire

#endif
