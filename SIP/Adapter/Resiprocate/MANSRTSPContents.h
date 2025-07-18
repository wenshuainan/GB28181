#ifndef MANSRTSP_CONTENTS_H
#define MANSRTSP_CONTENTS_H

#include "resip/stack/Contents.hxx"
#include "MANSRTSP/B.1Message.h"

using namespace resip;

/**
   @ingroup sip_payload
   @brief SIP body type for holding xml contents (MIME content-type Application/MANSCDP+xml).
*/
class MANSRTSPContents : public Contents
{
   public:
      static const MANSRTSPContents Empty;

      MANSRTSPContents();
      MANSRTSPContents(const MANSRTSP::Message& message);
      MANSRTSPContents(const Data& text);
      MANSRTSPContents(const HeaderFieldValue& hfv, const Mime& contentType);
      MANSRTSPContents(const Data& text, const Mime& contentType);
      MANSRTSPContents(const MANSRTSPContents& rhs);
      virtual ~MANSRTSPContents();
      MANSRTSPContents& operator=(const MANSRTSPContents& rhs);

      /** @brief duplicate an MANSRTSPContents object
          @return pointer to a new MANSRTSPContents object  
        **/
      virtual Contents* clone() const;

      static const Mime& getStaticType();

      virtual EncodeStream& encodeParsed(EncodeStream& str) const;
      virtual void parse(ParseBuffer& pb);

      const MANSRTSP::Message& message() const {checkParsed(); return mMessage;}
      MANSRTSP::Message& message() {checkParsed(); return mMessage;}

      static bool init();
   private:
      MANSRTSP::Message mMessage;
};

static bool invokePlainContentsInit = MANSRTSPContents::init();

#endif
