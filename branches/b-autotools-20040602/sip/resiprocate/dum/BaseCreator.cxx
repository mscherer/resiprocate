#include "DialogUsageManager.hxx"
#include "Profile.hxx"
#include "BaseCreator.hxx"
#include "resiprocate/Helper.hxx"

using namespace resip;

BaseCreator::BaseCreator(DialogUsageManager& dum) : mDum(dum)
{
}

BaseCreator::~BaseCreator()
{}

SipMessage& 
BaseCreator::getLastRequest()
{
   return mLastRequest;
}

void
BaseCreator::makeInitialRequest(const NameAddr& target, MethodTypes method)
{
   RequestLine rLine(method);
   rLine.uri().scheme() = target.uri().scheme();
   rLine.uri().host() = target.uri().host();
   rLine.uri().port() = target.uri().port();
   mLastRequest.header(h_RequestLine) = rLine;

   mLastRequest.header(h_To) = target;
   mLastRequest.header(h_MaxForwards).value() = 70;
   mLastRequest.header(h_CSeq).method() = method;
   mLastRequest.header(h_CSeq).sequence() = 1;
   mLastRequest.header(h_From) = mDum.getProfile()->getDefaultAor();
   mLastRequest.header(h_From).param(p_tag) = Helper::computeTag(Helper::tagSize);
   mLastRequest.header(h_CallId).value() = Helper::computeCallId();

   NameAddr contact; // if no GRUU, let the stack fill in the contact 
   if (mDum.getProfile()->hasGruu(target.uri().getAor()))
   {
      contact = mDum.getProfile()->getGruu(target.uri().getAor());
   }
   mLastRequest.header(h_Contacts).push_front(contact);
   
   Via via;
   mLastRequest.header(h_Vias).push_front(via);

   mLastRequest.header(h_Supporteds) = mDum.getProfile()->getSupportedOptionTags();
   mLastRequest.header(h_Accepts) = mDum.getProfile()->getSupportedMimeTypes();

   if (mDum.getProfile()->hasOutboundProxy())
   {
      assert(mLastRequest.header(h_Routes).empty());
      mLastRequest.header(h_Routes).push_back(mDum.getProfile()->getOutboundProxy());
   }
}

void
BaseCreator::dispatch(const SipMessage& msg)
{
   assert(0);
}