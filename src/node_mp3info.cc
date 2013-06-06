/*
 * Copyright (c) 2013, Marek Będkowski <marek@bedkowski.pl>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <v8.h>
#include <node.h>
#include <node_buffer.h>
#include "node_mp3info.h"
//#include "debug.h"

using namespace v8;
using namespace node;

namespace ns_mp3info {

char* stringArgToStr(const v8::Local<v8::Value> arg) { 
  v8::String::Utf8Value v8Str(arg); 
  char *cStr = (char*) malloc(strlen(*v8Str) + 1); 
  strcpy(cStr, *v8Str); 
  return cStr; 
}

Handle<Value> node_get_mp3_info(const Arguments& args) {
	char *filename = stringArgToStr(args[0]);
	int scantype=SCAN_QUICK, fullscan_vbr=1;
	FILE  *fp;
	mp3info mp3;
	
	if ( !( fp=fopen(filename,"rb") ) ) {
		return Undefined();
	}
	
	memset(&mp3,0,sizeof(mp3info));
	mp3.filename=filename;
	mp3.file=fp;

	get_mp3_info(&mp3,scantype,fullscan_vbr);
	
	return Number::New(mp3.seconds);
}

void InitMP3INFO(Handle<Object> target) {
  HandleScope scope;

#define CONST_INT(value) \
  target->Set(String::NewSymbol(#value), Integer::New(value), \
      static_cast<PropertyAttribute>(ReadOnly|DontDelete));	

  NODE_SET_METHOD(target, "get_mp3_info", 		node_get_mp3_info);
}

} // mp3info namespace
