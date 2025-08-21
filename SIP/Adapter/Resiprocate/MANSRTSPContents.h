#ifndef MANSRTSP_CONTENTS_H
#define MANSRTSP_CONTENTS_H

#include "resip/stack/Contents.hxx"
#include "MANSRTSP/B.1Message.h"

/**
   @ingroup sip_payload
   @brief SIP body type for holding xml contents (MIME content-type Application/MANSRTSP).
*/
class MANSRTSPContents : public resip::Contents
{
   public:
      static const MANSRTSPContents Empty;

      MANSRTSPContents();
      MANSRTSPContents(const MANSRTSP::Message& message);
      MANSRTSPContents(const resip::Data& text);
      MANSRTSPContents(const resip::HeaderFieldValue& hfv, const resip::Mime& contentType);
      MANSRTSPContents(const resip::Data& text, const resip::Mime& contentType);
      MANSRTSPContents(const MANSRTSPContents& rhs);
      virtual ~MANSRTSPContents();
      MANSRTSPContents& operator=(const MANSRTSPContents& rhs);

      /** @brief duplicate an MANSRTSPContents object
          @return pointer to a new MANSRTSPContents object  
        **/
      virtual resip::Contents* clone() const;

      static const resip::Mime& getStaticType();

      virtual EncodeStream& encodeParsed(EncodeStream& str) const;
      virtual void parse(resip::ParseBuffer& pb);

      const MANSRTSP::Message& message() const {checkParsed(); return mMessage;}
      MANSRTSP::Message& message() {checkParsed(); return mMessage;}

      static bool init();
   private:
      MANSRTSP::Message mMessage;
};

// static bool invokeMANSRTSPContentsInit = MANSRTSPContents::init();

#endif
