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
    static void fInit(Handle<Object> hoModule);
    
  private:
    explicit cWinPerfCounter(PDH_HQUERY hQuery, PDH_HCOUNTER hCounter) :
      _hQuery(hQuery),
      _hCounter(hCounter) {};
    
    ~cWinPerfCounter() {
      if (_hQuery) PdhCloseQuery(_hQuery);
    }
    
    static Persistent<Function> fConstructor;
    static NAN_METHOD(fNew);
    static NAN_METHOD(fnGetValue);
    PDH_HQUERY _hQuery = 0;
    PDH_HCOUNTER _hCounter = 0;
}

static void cWinPerfCounter::fInit(Handle<Object> hoExports) {
  NanScope();
  
  Local<FunctionTemplate> oConstructorFunctionTemplate = NanNew<FunctionTemplate>(fNew);
  oConstructorFunctionTemplate->SetClassName(NanNew<String>("cWinPerfCounter"));
  oConstructorFunctionTemplate->Set(
    NanNew<String>("sAddonNodeVersion"),                NanNew<String>(NODE_VERSION_STRING)
  );
  oConstructorFunctionTemplate->Set(
    NanNew<String>("sAddonNodeProcessorArchitecture"),  NanNew<String>(NODE_PROCESSOR_ARCHITECTURE)
  );
  oConstructorFunctionTemplate->InstanceTemplate()->SetInternalFieldCount(1);
  
  NODE_SET_PROTOTYPE_METHOD(oConstructorFunctionTemplate, "fnGetValue", fnGetValue);
  
  NanAssignPersistent(fConstructor, oConstructorFunctionTemplate->GetFunction());
  hoExports->Set(NanNew<String>("cWinPerfCounter"), oConstructorFunctionTemplate->GetFunction());
}

Persistent<Function> cWinPerfCounter::fConstructor;

NAN_METHOD(cWinPerfCounter::fNew) {
  NanScope();
  
  if (!args.IsConstructCall()) {
    Local<Value> avArguments[1] = { args[0] }; // move inline
    NanReturnValue(NanNew<Function>(fConstructor)->NewInstance(1, avArguments));
  }
  String::Value sPerfCounterName(args[0]);
  if (*sPerfCounterName == NULL) {
    NanThrowTypeError("Wrong type of argument");
    NanReturnUndefined();
  }
  PDH_HQUERY hQuery;
  if (PdhOpenQuery(NULL, 0, &hQuery) != ERROR_SUCCESS) {
    NanThrowError("PDH: Cannot open query");
    NanReturnUndefined();
  }
  PDH_HCOUNTER hCounter;
  if(PdhAddCounter(hQuery, (LPCWSTR)*sPerfCounterName, 0, &hCounter) != ERROR_SUCCESS) {
    PdhCloseQuery(hQuery);
    Local<String> sErrorMessage = String::Concat(
      NanNew<String>("PDH: Cannot add counter "),
      NanNew<String>(*sPerfCounterName)
    ); 
    NanThrowError(sErrorMessage);
    NanReturnUndefined();
  }
  cWinPerfCounter* oPerfCounter = new cWinPerfCounter(hQuery, hCounter);
  oPerfCounter->Wrap(args.This());
  NanReturnThis();
}

NAN_METHOD(cWinPerfCounter::fnGetValue) {
  NanScope();
  
  cWinPerfCounter* poPerfCounter = ObjectWrap::Unwrap<cWinPerfCounter>(args.Holder());
  if (PdhCollectQueryData(poPerfCounter->_hQuery) != ERROR_SUCCESS) {
    NanThrowError("PDH: Cannot collect query data");
    NanReturnUndefined();
  }
  PDH_FMT_COUNTERVALUE oCounterValue;
  PDH_STATUS xStatusCode = PdhGetFormattedCounterValue(poPerfCounter->_hCounter, PDH_FMT_DOUBLE | PDH_FMT_NOCAP100, 0, &oCounterValue);
  if (xStatusCode != ERROR_SUCCESS) {
    NanThrowError("PDH: Cannot format counter value");
    NanReturnUndefined();
  }
  if (oCounterValue.CStatus == PDH_CSTATUS_VALID_DATA) {
    NanReturnValue(NanNew<Number>(oCounterValue.doubleValue));
  }
  NanReturnNull();
}
/*
* Initialization functions that take two arguments were introduced in node.js
* version v0.9.8 and will not compile on lower version. To work around this,
* this code will use a single argument initialization function and the index.js
* file that loads the addon will copy the exports to the module.
* Code for v0.9.8 and up would be:
void fInit(Handle<Object> hoExports, Handle<Object> hoModule) {
  cWinPerfCounter::fInit(hoModule);
}
* Code for all versions, which requires index.js to copy the exports:
*/
void fInit(Handle<Object> hoExports) {
  cWinPerfCounter::fInit(hoExports);
}
NODE_MODULE(cWinPerfCounter, fInit)