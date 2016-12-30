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
      switch (oCounterValue.CStatus) {
        case PDH_CSTATUS_NO_MACHINE:
          Nan::ThrowError("PDH_CSTATUS_NO_MACHINE"); // Unable to connect to the computer specified in the counter path.
          info.GetReturnValue().SetNull();
          break;
        case PDH_CSTATUS_NO_OBJECT:
          Nan::ThrowError("PDH_CSTATUS_NO_OBJECT"); // The specified performance object coould not be found.
          info.GetReturnValue().SetNull();
          break;
        case PDH_CSTATUS_NO_INSTANCE:
          Nan::ThrowError("PDH_CSTATUS_NO_INSTANCE"); // The specified instance could not be found in the performance object.
          info.GetReturnValue().SetNull();
          break;
        case PDH_CSTATUS_NO_COUNTER:
          Nan::ThrowError("PDH_CSTATUS_NO_COUNTER"); //  The specified counter name could not be found.
          info.GetReturnValue().SetNull();
          break;
        case PDH_CSTATUS_INVALID_DATA:
          Nan::ThrowError("PDH_CSTATUS_INVALID_DATA"); // The counter returned invalid data.
          info.GetReturnValue().SetNull();
          break;
        case PDH_CSTATUS_VALID_DATA:
          info.GetReturnValue().Set(Nan::New<v8::Number>(oCounterValue.doubleValue));
          break;
        case PDH_CSTATUS_NEW_DATA:
          info.GetReturnValue().Set(Nan::New<v8::Number>(oCounterValue.doubleValue));
          break;
        case PDH_MORE_DATA:
          Nan::ThrowError("PDH_MORE_DATA"); // A larger buffer is needed to retrieve counter data.
          info.GetReturnValue().SetNull();
          break;
        case PDH_CSTATUS_ITEM_NOT_VALIDATED:
          Nan::ThrowError("PDH_CSTATUS_ITEM_NOT_VALIDATED"); // The counter but has not been validated nor accessed.
          info.GetReturnValue().SetNull();
          break;
        case PDH_CALC_NEGATIVE_DENOMINATOR:
          Nan::ThrowError("PDH_CALC_NEGATIVE_DENOMINATOR"); // A counter has a negative denominator value.
          info.GetReturnValue().SetNull();
          break;
        case PDH_CALC_NEGATIVE_TIMEBASE:
          Nan::ThrowError("PDH_CALC_NEGATIVE_TIMEBASE"); // A counter has a negative timebase value.
          info.GetReturnValue().SetNull();
          break;
        case PDH_CALC_NEGATIVE_VALUE:
          Nan::ThrowError("PDH_CALC_NEGATIVE_VALUE"); // A counter has a negative value.
          info.GetReturnValue().SetNull();
          break;
        case PDH_CSTATUS_NO_COUNTERNAME:
          Nan::ThrowError("PDH_CSTATUS_NO_COUNTERNAME"); // No counter path was specified in the query.
          info.GetReturnValue().SetNull();
          break;
        case PDH_CSTATUS_BAD_COUNTERNAME:
          Nan::ThrowError("PDH_CSTATUS_BAD_COUNTERNAME"); // The counter path format is incorrect.
          info.GetReturnValue().SetNull();
          break;
        default:
          Nan::ThrowError("PDH: Unknown CStatus value"); // I should probably add the value
          info.GetReturnValue().SetNull();
          break;
      };
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