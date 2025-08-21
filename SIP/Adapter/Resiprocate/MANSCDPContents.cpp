#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "MANSCDPContents.h"
#include "resip/stack/SipMessage.hxx"
#include "rutil/Logger.hxx"
#include "rutil/ParseBuffer.hxx"
#include "rutil/WinLeakCheck.hxx"

using namespace resip;
using namespace std;
using namespace tinyxml2;

#define RESIPROCATE_SUBSYSTEM Subsystem::APP

const MANSCDPContents MANSCDPContents::Empty;
static bool invokeMANSCDPContentsInit = MANSCDPContents::init();


bool
MANSCDPContents::init()
{
   static ContentsFactory<MANSCDPContents> factory;
   (void)factory;
   return true;
}


MANSCDPContents::MANSCDPContents()
   : Contents(getStaticType()),
     mXml()
{}

MANSCDPContents::MANSCDPContents(const XMLDocument& xml)
   : Contents(getStaticType())
{
   xml.DeepCopy(&mXml);
}

MANSCDPContents::MANSCDPContents(const Data& text)
   : Contents(getStaticType())
{
   mXml.Parse(text.c_str());
}

MANSCDPContents::MANSCDPContents(const HeaderFieldValue& hfv, const Mime& contentsType)
   : Contents(hfv, contentsType),
     mXml()
{
}
 
MANSCDPContents::MANSCDPContents(const Data& text, const Mime& contentsType)
   : Contents(contentsType),
     mXml()
{
   mXml.Parse(text.c_str());
}

MANSCDPContents::MANSCDPContents(const MANSCDPContents& rhs)
   : Contents(rhs),
     mXml()
{
   rhs.mXml.DeepCopy(&mXml);
}

MANSCDPContents::~MANSCDPContents()
{
}

MANSCDPContents&
MANSCDPContents::operator=(const MANSCDPContents& rhs)
{
   if (this != &rhs)
   {
      Contents::operator=(rhs);
      rhs.mXml.DeepCopy(&mXml);
   }
   return *this;
}

Contents* 
MANSCDPContents::clone() const
{
   return new MANSCDPContents(*this);
}

const Mime& 
MANSCDPContents::getStaticType() 
{
   static Mime type("Application","MANSCDP+xml");
   return type;
}

EncodeStream& 
MANSCDPContents::encodeParsed(EncodeStream& str) const
{
   //DebugLog(<< "MANSCDPContents::encodeParsed " << mXml);

   XMLPrinter printer;
   mXml.Print(&printer);
   str << printer.CStr();
   return str;
}

void 
MANSCDPContents::parse(ParseBuffer& pb)
{
   //DebugLog(<< "MANSCDPContents::parse: " << pb.position());

   Data data;
   const char* anchor = pb.position();
   pb.skipToEnd();
   pb.data(data, anchor);
   mXml.Parse(data.c_str());

   //DebugLog("MANSCDPContents::parsed <" << mXml << ">" );
}


/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */
