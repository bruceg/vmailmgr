#ifndef VMAILMGR__ERRORS__H__
#define VMAILMGR__ERRORS__H__

// This class serves as a namespace device, no data.
class error 
{
public:
  enum numbers {
    none = 0,
    cdbio,
    // This must stay at the end of the list
    errmax
  };
  static const char* str[errmax];
};

#endif // VMAILMGR__ERRORS__H__
