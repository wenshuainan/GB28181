#ifndef MANSCDP_CONTENTS_H
#define MANSCDP_CONTENTS_H

#include "resip/stack/Contents.hxx"
#include "tinyxml2.h"

using namespace resip;
using namespace tinyxml2;

/**
   @ingroup sip_payload
   @brief SIP body type for holding xml contents (MIME content-type Application/MANSCDP+xml).
*/
class MANSCDPContents : public Contents
{
   public:
      static const MANSCDPContents Empty;

      MANSCDPContents();
      MANSCDPContents(const XMLDocument& xml);
      MANSCDPContents(const Data& text);
      MANSCDPContents(const HeaderFieldValue& hfv, const Mime& contentType);
      MANSCDPContents(const Data& text, const Mime& contentType);
      MANSCDPContents(const MANSCDPContents& rhs);
      virtual ~MANSCDPContents();
      MANSCDPContents& operator=(const MANSCDPContents& rhs);

      /** @brief duplicate an MANSCDPContents object
          @return pointer to a new MANSCDPContents object  
        **/
      virtual Contents* clone() const;

      static const Mime& getStaticType();

      virtual EncodeStream& encodeParsed(EncodeStream& str) const;
      virtual void parse(ParseBuffer& pb);

      const XMLDocument& xml() const {checkParsed(); return mXml;}
      XMLDocument& xml() {checkParsed(); return mXml;}

      static bool init();
   private:
      XMLDocument mXml;
};

static bool invokePlainContentsInit = MANSCDPContents::init();

#endif
