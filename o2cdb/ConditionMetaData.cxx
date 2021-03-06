//  Set of data describing the object  				   //
//  but not used to identify the object 			   //
#include "ConditionMetaData.h"
#include <TObjString.h>   // for TObjString
#include <TTimeStamp.h>   // for TTimeStamp
#include <time.h>         // for time
#include "TCollection.h"  // for TIter
#include "THashTable.h"   // for THashTable
#include "TMap.h"         // for TMap, TPair
#include "TObject.h"      // for TObject

using namespace AliceO2::CDB;

ClassImp(ConditionMetaData)

ConditionMetaData::ConditionMetaData()
  : TObject(), mObjectClassName(""), mResponsible(""), mBeamPeriod(0), mAliRootVersion(""), mComment(""), mProperties()
{
  // default constructor

  mProperties.SetOwner(1);
}

ConditionMetaData::ConditionMetaData(const char* responsible, UInt_t beamPeriod, const char* alirootVersion, const char* comment)
  : TObject(),
    mObjectClassName(""),
    mResponsible(responsible),
    mBeamPeriod(beamPeriod),
    mAliRootVersion(alirootVersion),
    mComment(comment),
    mProperties()
{
  // constructor

  mProperties.SetOwner(1);
}

ConditionMetaData::~ConditionMetaData()
{
  // destructor
}

void ConditionMetaData::setProperty(const char* property, TObject* object)
{
  // add something to the list of properties

  mProperties.Add(new TObjString(property), object);
}

TObject* ConditionMetaData::getProperty(const char* property) const
{
  // get a property specified by its name (property)

  return mProperties.GetValue(property);
}

Bool_t ConditionMetaData::removeProperty(const char* property)
{
  // removes a property

  TObjString objStrProperty(property);
  TObjString* aKey = (TObjString*)mProperties.Remove(&objStrProperty);

  if (aKey) {
    delete aKey;
    return kTRUE;
  } else {
    return kFALSE;
  }
}

void ConditionMetaData::addDateToComment()
{
  // add the date to the comment.
  // This method is supposed to be useful if called at the time when the object
  // is created, so that later it can more easily be tracked, in particular
  // when the date of the file can be lost or when one is interested in the
  // date of creation, irrespective of a later copy of it

  TTimeStamp ts(time(0));
  TString comment(getComment());
  comment += Form("\tDate of production: %s\n", ts.AsString());
  comment.Remove(comment.Last('+'));
  setComment(comment);
}

void ConditionMetaData::printConditionMetaData()
{
  // print the object's metaData

  TString message;
  if (mObjectClassName != "")
    message += TString::Format("\tObject's class name:	%s\n", mObjectClassName.Data());
  if (mResponsible != "")
    message += TString::Format("\tResponsible:		%s\n", mResponsible.Data());
  if (mBeamPeriod != 0)
    message += TString::Format("\tBeam period:		%d\n", mBeamPeriod);
  if (mAliRootVersion != "")
    message += TString::Format("\tAliRoot version:	%s\n", mAliRootVersion.Data());
  if (mComment != "")
    message += TString::Format("\tComment:		%s\n", mComment.Data());
  if (mProperties.GetEntries() > 0) {
    message += "\tProperties key names:";

    TIter iter(mProperties.GetTable());
    TPair* aPair;
    while ((aPair = (TPair*)iter.Next())) {
      message += TString::Format("\t\t%s\n", ((TObjString*)aPair->Key())->String().Data());
    }
  }
  message += '\n';
  Printf("**** Object's ConditionMetaData parameters **** \n%s", message.Data());
}
