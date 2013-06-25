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

#include <string.h>
#include <stdlib.h>
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
    HandleScope scope;
    
	char *filename = stringArgToStr(args[0]);
    Local<Function> callback = Local<Function>::Cast(args[1]);
    
    mp3info_loop_data* data = new mp3info_loop_data;
    data->filename = filename;
    data->callback = Persistent<Function>::New(callback);
    data->scantype = SCAN_QUICK;
    data->fullscan_vbr = VBR_AVERAGE;
    
    data->req.data = data;
    
    uv_queue_work(uv_default_loop(), &data->req,
        node_get_mp3info_async,
        (uv_after_work_cb)node_get_mp3info_after);

    
    return Undefined();
}
    
void node_get_mp3info_async (uv_work_t *req) {
    mp3info_loop_data* data = (mp3info_loop_data*) req->data;
    
    FILE  *fp;
	
    data->error = NULL;
    if ( !( fp=fopen(data->filename, "rb") ) ) {
        data->error = new std::string("File not found.");
    } else {
        mp3info* mp3 = new mp3info;
	
        mp3->filename=data->filename;
    	mp3->file=fp;
    
    	get_mp3_info(mp3, data->scantype, data->fullscan_vbr);
        fclose(fp);
        data->mp3 = mp3;
    }
}

void node_get_mp3info_after (uv_work_t *req) {
    HandleScope scope;
    mp3info_loop_data* data = (mp3info_loop_data*) req->data;
    
    TryCatch try_catch;
    Local<Object> o = Object::New();
    Handle<Value> argv[2];
    if (data->error) {
        o->Set(String::NewSymbol("msg"), String::New(data->error->c_str(), data->error->length()));

        argv[0] = o;
        argv[1] = Undefined();
        data->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    } else {
        mp3info* mp3 = data->mp3;
        o->Set(String::NewSymbol("length"), Integer::New(mp3->seconds));
        {
            Local<Object> id3 = Object::New();
            id3->Set(String::NewSymbol("title"), String::NewSymbol(mp3->id3.title));
            id3->Set(String::NewSymbol("artist"), String::NewSymbol(mp3->id3.artist));
            id3->Set(String::NewSymbol("album"), String::NewSymbol(mp3->id3.album));
            id3->Set(String::NewSymbol("year"), String::NewSymbol(mp3->id3.year));
            id3->Set(String::NewSymbol("comment"), String::NewSymbol(mp3->id3.comment));
            o->Set(String::NewSymbol("id3"), id3);
        }
        {
            Local<Object> header = Object::New();
            
            header->Set(String::NewSymbol("sync"), Integer::New(mp3->header.sync));
            header->Set(String::NewSymbol("version"), Integer::New(mp3->header.version));
            header->Set(String::NewSymbol("layer"), Integer::New(mp3->header.layer));
            header->Set(String::NewSymbol("crc"), Integer::New(mp3->header.crc));
            header->Set(String::NewSymbol("bitrate"), Integer::New(mp3->header.bitrate));
            header->Set(String::NewSymbol("freq"), Integer::New(mp3->header.freq));
            header->Set(String::NewSymbol("padding"), Integer::New(mp3->header.padding));
            header->Set(String::NewSymbol("extension"), Integer::New(mp3->header.extension));
            header->Set(String::NewSymbol("mode"), Integer::New(mp3->header.mode));
            header->Set(String::NewSymbol("mode_extension"), Integer::New(mp3->header.mode_extension));
            header->Set(String::NewSymbol("copyright"), Integer::New(mp3->header.copyright));
            header->Set(String::NewSymbol("original"), Integer::New(mp3->header.original));
            header->Set(String::NewSymbol("emphasis"), Integer::New(mp3->header.emphasis));
            
            o->Set(String::NewSymbol("header"), header);
        }
        argv[0] = Undefined();
        argv[1] = o;
        data->callback->Call(Context::GetCurrent()->Global(), 2, argv);
    }
    
    data->callback.Dispose();
    delete data;
    
    if (try_catch.HasCaught()) {
        FatalException(try_catch);
    }
}


void InitMP3INFO(Handle<Object> target) {
  HandleScope scope;

#define CONST_INT(value) \
  target->Set(String::NewSymbol(#value), Integer::New(value), \
      static_cast<PropertyAttribute>(ReadOnly|DontDelete));
    
    CONST_INT(SCAN_NONE);
    CONST_INT(SCAN_QUICK);
    CONST_INT(SCAN_FULL);
    
    CONST_INT(VBR_VARIABLE);
    CONST_INT(VBR_AVERAGE);
    CONST_INT(VBR_MEDIAN);
    
  NODE_SET_METHOD(target, "get_mp3_info", 		node_get_mp3_info);
}

} // mp3info namespace
