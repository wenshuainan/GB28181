#ifndef MANSCDP_CONTENTS_H
#define MANSCDP_CONTENTS_H

#include "resip/stack/Contents.hxx"

using namespace resip;

/**
   @ingroup sip_payload
   @brief SIP body type for holding xml contents (MIME content-type Application/MANSCDP+xml).
*/
class MANSCDPContents : public Contents
{
   public:
      static const MANSCDPContents Empty;

      MANSCDPContents();
      MANSCDPContents(const Data& text);
      MANSCDPContents(const HeaderFieldValue& hfv, const Mime& contentType);
      MANSCDPContents(const Data& data, const Mime& contentType);
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

      const Data& xml() const {checkParsed(); return mXml;}
      Data& xml() {checkParsed(); return mXml;}

      static bool init();
   private:
      Data mXml;
};

static bool invokePlainContentsInit = MANSCDPContents::init();

#endif
