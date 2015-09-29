#define _UNICODE 
#include <assert.h>
#include <nan.h>
#include <node.h>
#include <node_object_wrap.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <windows.h>

#ifdef _M_IX86
  #define NODE_PROCESSOR_ARCHITECTURE "ia32"
#endif
#ifdef _M_X64
  #define NODE_PROCESSOR_ARCHITECTURE "x64"
#endif

using namespace v8;

class cWinPerfCounter : public node::ObjectWrap {
  public:
    static void fInit(Local<Object> hoModule);
    
  private:
    explicit cWinPerfCounter(PDH_HQUERY hQuery, PDH_HCOUNTER hCounter) :
      _hQuery(hQuery),
      _hCounter(hCounter) {};
    
    ~cWinPerfCounter() {
      if (_hQuery) PdhCloseQuery(_hQuery);
    }
    
    static Nan::Persistent<Function> fConstructor;
    static NAN_METHOD(fNew);
    static NAN_METHOD(fnGetValue);
    PDH_HQUERY _hQuery = 0;
    PDH_HCOUNTER _hCounter = 0;
}

static void cWinPerfCounter::fInit(Local<Object> hoExports) {
  Nan::HandleScope scope;
  
  Local<FunctionTemplate> oConstructorFunctionTemplate = Nan::New<FunctionTemplate>(fNew);
  oConstructorFunctionTemplate->SetClassName(Nan::New<String>("cWinPerfCounter").ToLocalChecked());
  oConstructorFunctionTemplate->Set(
    Nan::New<String>("sAddonNodeVersion").ToLocalChecked(),                Nan::New<String>(NODE_VERSION_STRING).ToLocalChecked()
  );
  oConstructorFunctionTemplate->Set(
    Nan::New<String>("sAddonNodeProcessorArchitecture").ToLocalChecked(),  Nan::New<String>(NODE_PROCESSOR_ARCHITECTURE).ToLocalChecked()
  );
  oConstructorFunctionTemplate->InstanceTemplate()->SetInternalFieldCount(1);
  
  Nan::SetPrototypeMethod(oConstructorFunctionTemplate, "fnGetValue", fnGetValue);
  
  fConstructor.Reset(oConstructorFunctionTemplate->GetFunction());
  hoExports->Set(Nan::New<String>("cWinPerfCounter").ToLocalChecked(), oConstructorFunctionTemplate->GetFunction());
}

Nan::Persistent<Function> cWinPerfCounter::fConstructor;

NAN_METHOD(cWinPerfCounter::fNew) {
  Nan::HandleScope scope;
  
  if (!info.IsConstructCall()) {
    Local<Value> avArguments[1] = { info[0] }; // move inline
    info.GetReturnValue().Set(Nan::New<Function>(fConstructor)->NewInstance(1, avArguments));
  }
  String::Value sPerfCounterName(info[0]);
  if (*sPerfCounterName == NULL) {
    Nan::ThrowTypeError("Wrong type of argument");
    info.GetReturnValue().SetUndefined();
  }
  PDH_HQUERY hQuery;
  if (PdhOpenQuery(NULL, 0, &hQuery) != ERROR_SUCCESS) {
    Nan::ThrowError("PDH: Cannot open query");
    info.GetReturnValue().SetUndefined();
  }
  PDH_HCOUNTER hCounter;
  if(PdhAddCounter(hQuery, (LPCWSTR)*sPerfCounterName, 0, &hCounter) != ERROR_SUCCESS) {
    PdhCloseQuery(hQuery);
    Local<String> sErrorMessage = String::Concat(
      Nan::New<String>("PDH: Cannot add counter ").ToLocalChecked(),
      Nan::New<String>(*sPerfCounterName).ToLocalChecked()
    ); 
    Nan::ThrowError(sErrorMessage);
    info.GetReturnValue().SetUndefined();
  }
  cWinPerfCounter* oPerfCounter = new cWinPerfCounter(hQuery, hCounter);
  oPerfCounter->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(cWinPerfCounter::fnGetValue) {
  Nan::HandleScope scope;
  
  cWinPerfCounter* poPerfCounter = ObjectWrap::Unwrap<cWinPerfCounter>(info.Holder());
  if (PdhCollectQueryData(poPerfCounter->_hQuery) != ERROR_SUCCESS) {
    Nan::ThrowError("PDH: Cannot collect query data");
    info.GetReturnValue().SetUndefined();
  }
  PDH_FMT_COUNTERVALUE oCounterValue;
  PDH_STATUS xStatusCode = PdhGetFormattedCounterValue(poPerfCounter->_hCounter, PDH_FMT_DOUBLE | PDH_FMT_NOCAP100, 0, &oCounterValue);
  if (xStatusCode != ERROR_SUCCESS) {
    Nan::ThrowError("PDH: Cannot format counter value");
    info.GetReturnValue().SetUndefined();
  }
  if (oCounterValue.CStatus == PDH_CSTATUS_VALID_DATA) {
    info.GetReturnValue().Set(Nan::New<Number>(oCounterValue.doubleValue));
  }
}
/*
* Initialization functions that take two arguments were introduced in node.js
* version v0.9.8 and will not compile on lower version. To work around this,
* this code will use a single argument initialization function and the index.js
* file that loads the addon will copy the exports to the module.
* Code for v0.9.8 and up would be:
void fInit(Local<Object> hoExports, Local<Object> hoModule) {
  cWinPerfCounter::fInit(hoModule);
}
* Code for all versions, which requires index.js to copy the exports:
*/
void fInit(Local<Object> hoExports) {
  cWinPerfCounter::fInit(hoExports);
}
NODE_MODULE(cWinPerfCounter, fInit)