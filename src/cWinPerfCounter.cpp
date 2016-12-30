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

class cWinPerfCounter : public Nan::ObjectWrap {
  private:
    explicit cWinPerfCounter(
      PDH_HQUERY hQuery,
      PDH_HCOUNTER hCounter
    ) :
      _hQuery(hQuery),
      _hCounter(hCounter)
    { };
    
    ~cWinPerfCounter() {
      if (_hQuery) PdhCloseQuery(_hQuery);
    };
    
    PDH_HQUERY _hQuery = 0;
    PDH_HCOUNTER _hCounter = 0;
    
    static inline Nan::Persistent<v8::Function> & ffGetConstructor() {;
      static Nan::Persistent<v8::Function> fConstructor;
      return fConstructor;
    };
    static NAN_METHOD(fNew) {
      if (!info.IsConstructCall()) {
        v8::Local<v8::Value> avArguments[1] = { info[0] }; // move inline
        v8::Local<v8::Object> oWinPerfCounter = Nan::NewInstance(Nan::New(ffGetConstructor()), 1, avArguments).ToLocalChecked();
        info.GetReturnValue().Set(oWinPerfCounter);
      }
      v8::String::Value sPerfCounterName(info[0]);
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
        v8::Local<v8::String> sErrorMessage = v8::String::Concat(
          Nan::New<v8::String>("PDH: Cannot add counter ").ToLocalChecked(),
          Nan::New<v8::String>(*sPerfCounterName).ToLocalChecked()
        ); 
        Nan::ThrowError(sErrorMessage);
        info.GetReturnValue().SetUndefined();
      }
      cWinPerfCounter* oPerfCounter = new cWinPerfCounter(hQuery, hCounter);
      oPerfCounter->Wrap(info.This());
      info.GetReturnValue().Set(info.This());
    };
    
    static NAN_METHOD(fnGetValue) {
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
        info.GetReturnValue().Set(Nan::New<v8::Number>(oCounterValue.doubleValue));
      }
      info.GetReturnValue().SetNull();
    };
  
  public:
    static NAN_MODULE_INIT(fInit) {
      v8::Local<v8::FunctionTemplate> oConstructorFunctionTemplate = Nan::New<v8::FunctionTemplate>(fNew);
      oConstructorFunctionTemplate->SetClassName(
        Nan::New("cWinPerfCounter").ToLocalChecked()
      );
      oConstructorFunctionTemplate->InstanceTemplate()->SetInternalFieldCount(1);
      
      oConstructorFunctionTemplate->Set(
        Nan::New("sAddonNodeVersion").ToLocalChecked(),
        Nan::New(NODE_VERSION_STRING).ToLocalChecked()
      );
      oConstructorFunctionTemplate->Set(
        Nan::New("sAddonNodeProcessorArchitecture").ToLocalChecked(),
        Nan::New(NODE_PROCESSOR_ARCHITECTURE).ToLocalChecked()
      );
      
      Nan::SetPrototypeMethod(oConstructorFunctionTemplate, "fnGetValue", fnGetValue);
      
      // Create the constructor
      ffGetConstructor().Reset(Nan::GetFunction(oConstructorFunctionTemplate).ToLocalChecked());
//      NAN_EXPORT(cWinPerfCounter, cWinPerfCounter::fNew));
      Nan::Set(
        target,
        Nan::New("cWinPerfCounter").ToLocalChecked(),
        Nan::GetFunction(oConstructorFunctionTemplate).ToLocalChecked()
      );
    };
};

NODE_MODULE(cWinPerfCounter, cWinPerfCounter::fInit)