#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "MANSRTSPContents.h"
#include "resip/stack/SipMessage.hxx"
#include "rutil/Logger.hxx"
#include "rutil/ParseBuffer.hxx"
#include "rutil/WinLeakCheck.hxx"

using namespace resip;
using namespace std;

#define RESIPROCATE_SUBSYSTEM Subsystem::SIP

const MANSRTSPContents MANSRTSPContents::Empty;
static bool invokeMANSRTSPContentsInit = MANSRTSPContents::init();


bool
MANSRTSPContents::init()
{
   static ContentsFactory<MANSRTSPContents> factory;
   (void)factory;
   return true;
}


MANSRTSPContents::MANSRTSPContents()
   : Contents(getStaticType()),
     mMessage()
{}

MANSRTSPContents::MANSRTSPContents(const MANSRTSP::Message& message)
   : Contents(getStaticType())
{
   mMessage = message;
}

MANSRTSPContents::MANSRTSPContents(const Data& text)
   : Contents(getStaticType())
{
   mMessage.parse(text.c_str(), text.size());
}

MANSRTSPContents::MANSRTSPContents(const HeaderFieldValue& hfv, const Mime& contentsType)
   : Contents(hfv, contentsType),
     mMessage()
{
}
 
MANSRTSPContents::MANSRTSPContents(const Data& text, const Mime& contentsType)
   : Contents(contentsType)
{
   mMessage.parse(text.c_str(), text.size());
}

MANSRTSPContents::MANSRTSPContents(const MANSRTSPContents& rhs)
   : Contents(rhs)
{
   mMessage = rhs.mMessage;
}

MANSRTSPContents::~MANSRTSPContents()
{
}

MANSRTSPContents&
MANSRTSPContents::operator=(const MANSRTSPContents& rhs)
{
   if (this != &rhs)
   {
      Contents::operator=(rhs);
      mMessage = rhs.mMessage;
   }
   return *this;
}

Contents* 
MANSRTSPContents::clone() const
{
   return new MANSRTSPContents(*this);
}

const Mime& 
MANSRTSPContents::getStaticType() 
{
   static Mime type("Application","MANSRTSP");
   return type;
}

EncodeStream& 
MANSRTSPContents::encodeParsed(EncodeStream& str) const
{
   //DebugLog(<< "MANSRTSPContents::encodeParsed " << mMessage);

   std::string data;
   mMessage.encode(data);
   str << data;
   return str;
}

void 
MANSRTSPContents::parse(ParseBuffer& pb)
{
   //DebugLog(<< "MANSRTSPContents::parse: " << pb.position());

   Data data;
   const char* anchor = pb.position();
   pb.skipToEnd();
   pb.data(data, anchor);
   mMessage.parse(data.c_str(), data.size());

   //DebugLog("MANSRTSPContents::parsed <" << mMessage << ">" );
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
