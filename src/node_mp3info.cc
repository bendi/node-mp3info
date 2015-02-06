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
#include <uv.h>
#include <node.h>
#include <node_buffer.h>
#include "node_mp3info.h"
//#include "debug.h"
#include "nan.h"

using namespace v8;
using namespace node;

namespace ns_mp3info {

char* stringArgToStr(const v8::Local<v8::Value> arg) { 
  v8::String::Utf8Value v8Str(arg); 
  char *cStr = (char*) malloc(strlen(*v8Str) + 1); 
  strcpy(cStr, *v8Str); 
  return cStr; 
}

NAN_METHOD(node_get_mp3_info) {
    NanScope();
    
    char *filename = stringArgToStr(args[0]);
    
    mp3info_loop_data* data = new mp3info_loop_data;
    data->filename = filename;
    NanAssignPersistent(data->callback, args[1].As<Function>());
    data->scantype = SCAN_QUICK;
    data->fullscan_vbr = VBR_AVERAGE;
    
    data->req.data = data;
    
    uv_queue_work(uv_default_loop(), &data->req,
        node_get_mp3info_async,
        (uv_after_work_cb)node_get_mp3info_after);

    
    NanReturnUndefined();
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
    NanScope();
    mp3info_loop_data* data = (mp3info_loop_data*) req->data;
    
    TryCatch try_catch;
    Local<Object> o = NanNew<Object>();
    Handle<Value> argv[2];
    if (data->error) {
        o->Set(NanNew<String>("msg"), NanNew<String>(data->error->c_str(), data->error->length()));

        argv[0] = o;
        argv[1] = NanUndefined();
    } else {
        mp3info* mp3 = data->mp3;
        o->Set(NanNew<String>("length"), NanNew<Integer>(mp3->seconds));
        {
            Local<Object> id3 = NanNew<Object>();
            id3->Set(NanNew<String>("title"), NanNew<String>(mp3->id3.title));
            id3->Set(NanNew<String>("artist"), NanNew<String>(mp3->id3.artist));
            id3->Set(NanNew<String>("album"), NanNew<String>(mp3->id3.album));
            id3->Set(NanNew<String>("year"), NanNew<String>(mp3->id3.year));
            id3->Set(NanNew<String>("comment"), NanNew<String>(mp3->id3.comment));
            o->Set(NanNew<String>("id3"), id3);
        }
        {
            Local<Object> header = NanNew<Object>();
            
            header->Set(NanNew<String>("sync"), NanNew<Number>(mp3->header.sync));
            header->Set(NanNew<String>("version"), NanNew<Integer>(mp3->header.version));
            header->Set(NanNew<String>("layer"), NanNew<Integer>(mp3->header.layer));
            header->Set(NanNew<String>("crc"), NanNew<Integer>(mp3->header.crc));
            header->Set(NanNew<String>("bitrate"), NanNew<Integer>(mp3->header.bitrate));
            header->Set(NanNew<String>("freq"), NanNew<Integer>(mp3->header.freq));
            header->Set(NanNew<String>("padding"), NanNew<Integer>(mp3->header.padding));
            header->Set(NanNew<String>("extension"), NanNew<Integer>(mp3->header.extension));
            header->Set(NanNew<String>("mode"), NanNew<Integer>(mp3->header.mode));
            header->Set(NanNew<String>("mode_extension"), NanNew<Integer>(mp3->header.mode_extension));
            header->Set(NanNew<String>("copyright"), NanNew<Integer>(mp3->header.copyright));
            header->Set(NanNew<String>("original"), NanNew<Integer>(mp3->header.original));
            header->Set(NanNew<String>("emphasis"), NanNew<Integer>(mp3->header.emphasis));
            
            o->Set(NanNew<String>("header"), header);
        }
        argv[0] = NanUndefined();
        argv[1] = o;
    }
    
    NanNew(data->callback)->Call(NanGetCurrentContext()->Global(), 2, argv);
    NanDisposePersistent(data->callback);
    delete data;
    
    if (try_catch.HasCaught()) {
        FatalException(try_catch);
    }
}


void InitMP3INFO(Handle<Object> target) {
  NanScope();

#define CONST_INT(value) \
  target->ForceSet(NanNew<String>(#value), NanNew<Integer>(value), \
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
