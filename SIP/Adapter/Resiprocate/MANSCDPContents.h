#ifndef MANSCDP_CONTENTS_H
#define MANSCDP_CONTENTS_H

#include "resip/stack/Contents.hxx"
#include "tinyxml2/tinyxml2.h"

/**
   @ingroup sip_payload
   @brief SIP body type for holding xml contents (MIME content-type Application/MANSCDP+xml).
*/
class MANSCDPContents : public resip::Contents
{
   public:
      static const MANSCDPContents Empty;

      MANSCDPContents();
      MANSCDPContents(const tinyxml2::XMLDocument& xml);
      MANSCDPContents(const resip::Data& text);
      MANSCDPContents(const resip::HeaderFieldValue& hfv, const resip::Mime& contentType);
      MANSCDPContents(const resip::Data& text, const resip::Mime& contentType);
      MANSCDPContents(const MANSCDPContents& rhs);
      virtual ~MANSCDPContents();
      MANSCDPContents& operator=(const MANSCDPContents& rhs);

      /** @brief duplicate an MANSCDPContents object
          @return pointer to a new MANSCDPContents object  
        **/
      virtual resip::Contents* clone() const;

      static const resip::Mime& getStaticType();

      virtual EncodeStream& encodeParsed(EncodeStream& str) const;
      virtual void parse(resip::ParseBuffer& pb);

      const tinyxml2::XMLDocument& xml() const {checkParsed(); return mXml;}
      tinyxml2::XMLDocument& xml() {checkParsed(); return mXml;}

      static bool init();
   private:
      tinyxml2::XMLDocument mXml;
};

// static bool invokeMANSCDPContentsInit = MANSCDPContents::init();

#endif
