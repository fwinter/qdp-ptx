// $Id: qdp_io.cc,v 1.3 2003-06-04 19:41:50 edwards Exp $
//
// QDP data parallel interface
//

#include "qdp.h"

namespace QDPUtil
{
  // Useful prototypes
  bool big_endian();
  void byte_swap(void *ptr, size_t size, size_t nmemb);
}

QDP_BEGIN_NAMESPACE(QDP);


//-----------------------------------------
//! text reader support
TextReader::TextReader() {iop=false;}

TextReader::TextReader(const std::string& p) {open(p);}

void TextReader::open(const std::string& p) 
{
  if (Layout::primaryNode())
  {
    f.open(p.c_str(),std::ifstream::in);
    if (! f.is_open())
      QDP_error_exit("failed to open file %s",p.c_str());
  }

  iop=true;
}

void TextReader::close()
{
  if (is_open()) 
  {
    if (Layout::primaryNode()) 
      f.close();
    iop = false;
  }
}

bool TextReader::is_open() {return iop;}

TextReader::~TextReader() {close();}


//-----------------------------------------
//! text writer support
TextWriter::TextWriter() {iop=false;}

TextWriter::TextWriter(const std::string& p) {open(p);}

void TextWriter::open(const std::string& p)
{
  if (Layout::primaryNode())
  {
    f.open(p.c_str(),std::ofstream::out);
    if (! f.is_open())
      QDP_error_exit("failed to open file %s",p.c_str());
  }

  iop=true;
}

void TextWriter::close()
{
  if (is_open()) 
  {
    if (Layout::primaryNode()) 
      f.close();
    iop = false;
  }
}

bool TextWriter::is_open() {return iop;}

TextWriter::~TextWriter() {close();}


//-----------------------------------------
//! text reader support
NmlReader::NmlReader() {abs = NULL; iop = false; stack_cnt = 0;}

NmlReader::NmlReader(const std::string& p) {abs = NULL; iop = false; stack_cnt = 0; open(p);}

void NmlReader::open(const std::string& p)
{
  abs = NULL;

  // Make a barrier call ?

  if (Layout::primaryNode()) 
  {
    ifstream f;

    f.open(p.c_str(),std::ifstream::in);
    if (! f.is_open())
      QDP_error_exit("NmlReader: error opening file %s",p.c_str());

    if ((abs = new_abstract("abstract")) == NULL)   // create a parse tree
      QDP_error_exit("NmlReader: Error initializing file - %s - for reading",p.c_str());
    
    // Parse from string
    ostringstream ss;
    f >> ss.rdbuf();
    if (param_scan_arg(abs, ss.str().c_str()) != 0)
      QDP_error_exit("NmlReader: Error scaning namelist file - %s - for reading",p.c_str());

    f.close();

    init_nml_section_stack(abs);
  }

  // Make a barrier call ?

  iop=true;
}

void NmlReader::close()
{
  if (iop)
  {
    while(stack_cnt > 0)
      pop();

    if (Layout::primaryNode()) 
      rm_abstract(abs);

    iop = false;
  }
}

bool NmlReader::is_open() {return iop;}

NmlReader::~NmlReader()
{
  close();
}

//! Push a namelist group 
NmlReader& NmlReader::push(const string& s)
{
  ++stack_cnt;

  if (Layout::primaryNode()) 
    push_nml_section_stack(s.c_str());

  return *this;
}

//! Pop a namelist group
NmlReader& NmlReader::pop()
{
  stack_cnt--;

  if (Layout::primaryNode()) 
    pop_nml_section_stack();

  return *this;
}

//! Push a namelist group 
NmlReader& push(NmlReader& nml, const string& s) {return nml.push(s);}

//! Pop a namelist group
NmlReader& pop(NmlReader& nml) {return nml.pop();}


//! Function overload read of  multi1d<int>
NmlReader& read(NmlReader& nml, const string& s, multi1d<int>& d)
{
  for(int i=0; i < d.size(); ++i)
    read(nml, s, d[i], i);
  return nml;
}

//! Function overload read of  multi1d<float>
NmlReader& read(NmlReader& nml, const string& s, multi1d<float>& d)
{
  for(int i=0; i < d.size(); ++i)
    read(nml, s, d[i], i);
  return nml;
}

//! Function overload read of  multi1d<double>
NmlReader& read(NmlReader& nml, const string& s, multi1d<double>& d)
{
  for(int i=0; i < d.size(); ++i)
    read(nml, s, d[i], i);
  return nml;
}


//! Function overload read of  Integer
NmlReader& read(NmlReader& nml, const string& s, Integer& d)
{
  WordType<Integer>::Type_t  dd;
  read(nml,s,dd);
  d = dd;

  return nml;
}

//! Function overload read of  Real
NmlReader& read(NmlReader& nml, const string& s, Real& d)
{
  WordType<Real>::Type_t  dd;
  read(nml,s,dd);
  d = dd;

  return nml;
}

//! Function overload read of  Double
NmlReader& read(NmlReader& nml, const string& s, Double& d)
{
  WordType<Double>::Type_t  dd;
  read(nml,s,dd);
  d = dd;

  return nml;
}

//! Function overload read of  Boolean
NmlReader& read(NmlReader& nml, const string& s, Boolean& d)
{
  WordType<Boolean>::Type_t  dd;
  read(nml,s,dd);
  d = dd;

  return nml;
}

//! Function overload read of  multi1d<Integer>
NmlReader& read(NmlReader& nml, const string& s, multi1d<Integer>& d)
{
  WordType<Integer>::Type_t  dd;

  for(int i=0; i < d.size(); ++i)
  {
    read(nml,s,dd,i);
    d[i] = dd;
  }

  return nml;
}

//! Function overload read of  multi1d<Real>
NmlReader& read(NmlReader& nml, const string& s, multi1d<Real>& d)
{
  WordType<Real>::Type_t  dd;

  for(int i=0; i < d.size(); ++i)
  {
    read(nml,s,dd,i);
    d[i] = dd;
  }

  return nml;
}

//! Function overload read of  multi1d<Double>
NmlReader& read(NmlReader& nml, const string& s, multi1d<Double>& d)
{
  WordType<Double>::Type_t  dd;

  for(int i=0; i < d.size(); ++i)
  {
    read(nml,s,dd,i);
    d[i] = dd;
  }

  return nml;
}


//-----------------------------------------
//! namelist writer support
NmlWriter::NmlWriter() {iop=false; stack_cnt = 0;}

NmlWriter::NmlWriter(const std::string& p) {stack_cnt = 0; open(p);}

void NmlWriter::open(const std::string& p)
{
  if (Layout::primaryNode())
  {
    f.open(p.c_str(),std::ofstream::out);
    if (! f.is_open())
      QDP_error_exit("failed to open file %s",p.c_str());
  }

  iop=true;

//  push(*this,"FILE");  // Always start a file with this group
}

void NmlWriter::close()
{
  if (iop) 
  {
//    pop(*this);  // Write final end of file group

    while(stack_cnt > 0)
      pop();

    if (Layout::primaryNode()) 
      f.close();
    iop = false;
  }
}

bool NmlWriter::is_open() {return iop;}

NmlWriter::~NmlWriter()
{
  close();
}

//! Push a namelist group 
NmlWriter& NmlWriter::push(const string& s)
{
  ++stack_cnt;

  if (Layout::primaryNode()) 
    get() << "&" << s << endl; 

  return *this;
}

//! Pop a namelist group
NmlWriter& NmlWriter::pop()
{
  stack_cnt--;

  if (Layout::primaryNode()) 
    get() << "&END\n"; 

  return *this;
}

//! Push a namelist group 
NmlWriter& push(NmlWriter& nml, const string& s) {return nml.push(s);}

//! Pop a namelist group
NmlWriter& pop(NmlWriter& nml) {return nml.pop();}

//! Write a comment
NmlWriter& operator<<(NmlWriter& nml, const char* s)
{
  if (Layout::primaryNode()) 
    nml.get() << "! " << s << endl; 

  return nml;
}

//! Write a comment
NmlWriter& operator<<(NmlWriter& nml, const std::string& s)
{
  if (Layout::primaryNode()) 
    nml.get() << "! " << s << endl; 

  return nml;
}



//-----------------------------------------
//! Binary reader support
BinaryReader::BinaryReader() {iop=false;}

BinaryReader::BinaryReader(const std::string& p) {open(p);}

void BinaryReader::open(const std::string& p) 
{
  if (Layout::primaryNode()) 
  {
    f.open(p.c_str(),std::ifstream::in | std::ifstream::binary);
    if (! f.is_open())
      QDP_error_exit("BinaryReader: error opening file %s",p.c_str());
  }

  iop = true;
}

void BinaryReader::close()
{
  if (iop)
  {
    if (Layout::primaryNode()) 
      f.close();

    iop = false;
  }
}


bool BinaryReader::is_open() {return iop;}

BinaryReader::~BinaryReader() {close();}

// Wrappers for read functions
void read(BinaryReader& bin, std::string& input, size_t maxBytes)
{
  bin.read(input, maxBytes);
}

void read(BinaryReader& bin, char& input)
{
  bin.read(input);
}

void read(BinaryReader& bin, int& input)
{
  bin.read(input);
}

void read(BinaryReader& bin, unsigned int& input)
{
  bin.read(input);
}

void read(BinaryReader& bin, short int& input)
{
  bin.read(input);
}

void read(BinaryReader& bin, unsigned short int& input)
{
  bin.read(input);
}

void read(BinaryReader& bin, long int& input)
{
  bin.read(input);
}

void read(BinaryReader& bin, unsigned long int& input)
{
  bin.read(input);
}

void read(BinaryReader& bin, float& input)
{
  bin.read(input);
}

void read(BinaryReader& bin, double& input)
{
  bin.read(input);
}

void read(BinaryReader& bin, bool& input)
{
  bin.read(input);
}

// Different bindings for read functions
BinaryReader& operator>>(BinaryReader& bin, char& input)
{
  read(bin, input);
  return bin;
}

BinaryReader& operator>>(BinaryReader& bin, int& input)
{
  read(bin, input);
  return bin;
}

BinaryReader& operator>>(BinaryReader& bin, unsigned int& input)
{
  read(bin, input);
  return bin;
}

BinaryReader& operator>>(BinaryReader& bin, short int& input)
{
  read(bin, input);
  return bin;
}

BinaryReader& operator>>(BinaryReader& bin, unsigned short int& input)
{
  read(bin, input);
  return bin;
}

BinaryReader& operator>>(BinaryReader& bin, long int& input)
{
  read(bin, input);
  return bin;
}

BinaryReader& operator>>(BinaryReader& bin, unsigned long int& input)
{
  read(bin, input);
  return bin;
}

BinaryReader& operator>>(BinaryReader& bin, float& input)
{
  read(bin, input);
  return bin;
}

BinaryReader& operator>>(BinaryReader& bin, double& input)
{
  read(bin, input);
  return bin;
}

BinaryReader& operator>>(BinaryReader& bin, bool& input)
{
  read(bin, input);
  return bin;
}

void BinaryReader::read(string& input, size_t maxBytes)
{
  char *str = new char[maxBytes];
  size_t n;

  if (Layout::primaryNode())
  {
    getIstream().getline(str, maxBytes);
    n = strlen(str)+1;
  }

  Internal::broadcast(n);
  Internal::broadcast((void *)str, n);

  input = str;
  delete[] str;
}

void BinaryReader::read(char& input) 
{
  readPrimitive<char>(input);
}

void BinaryReader::read(int& input) 
{
  readPrimitive<int>(input);
}

void BinaryReader::read(unsigned int& input)
{
  readPrimitive<unsigned int>(input);
}

void BinaryReader::read(short int& input)
{
  readPrimitive<short int>(input);
}

void BinaryReader::read(unsigned short int& input)
{
  readPrimitive<unsigned short int>(input);
}

void BinaryReader::read(long int& input)
{
  readPrimitive<long int>(input);
}

void BinaryReader::read(unsigned long int& input)
{
  readPrimitive<unsigned long int>(input);
}

void BinaryReader::read(float& input)
{
  readPrimitive<float>(input);
}

void BinaryReader::read(double& input)
{
  readPrimitive<double>(input);
}

void BinaryReader::read(bool& input)
{
  readPrimitive<bool>(input);
}

template< typename T>
void BinaryReader::readPrimitive(T& input)
{
  readArray((char*)&input, sizeof(T), 1);

  // Now broadcast back out to all nodes
  Internal::broadcast(input);
}

void BinaryReader::readArray(char* input, size_t size, size_t nmemb)
{
  if (Layout::primaryNode())
  {
    if (QDPUtil::big_endian())
    {
      /* big-endian */
      /* Write */
      getIstream().read(input, size*nmemb);
    }
    else
    {
      /* little-endian */
      /* Swap and write and swap */
      QDPUtil::byte_swap(input, size, nmemb);
      getIstream().read(input, size*nmemb);
      QDPUtil::byte_swap(input, size, nmemb);
    }
  }
}


//-----------------------------------------
//! Binary writer support
BinaryWriter::BinaryWriter() {iop=false;}

BinaryWriter::BinaryWriter(const std::string& p) {open(p);}

void BinaryWriter::open(const std::string& p) 
{
  if (Layout::primaryNode()) 
  {
    f.open(p.c_str(),std::ofstream::out | std::ofstream::binary);
    if (! f.is_open())
      QDP_error_exit("BinaryWriter: error opening file %s",p.c_str());
  }

  iop = true;
}

void BinaryWriter::close()
{
  if (iop)
  {
    if (Layout::primaryNode()) 
      f.close();

    iop = false;
  }
}


bool BinaryWriter::is_open() {return iop;}

BinaryWriter::~BinaryWriter() {close();}

// Wrappers for read functions
void write(BinaryWriter& bin, const std::string& output)
{
  bin.write(output);
}

void write(BinaryWriter& bin, const char& output)
{
  bin.write(output);
}

void write(BinaryWriter& bin, const int& output)
{
  bin.write(output);
}

void write(BinaryWriter& bin, const unsigned int& output)
{
  bin.write(output);
}

void write(BinaryWriter& bin, const short int& output)
{
  bin.write(output);
}

void write(BinaryWriter& bin, const unsigned short int& output)
{
  bin.write(output);
}

void write(BinaryWriter& bin, const long int& output)
{
  bin.write(output);
}

void write(BinaryWriter& bin, const unsigned long int& output)
{
  bin.write(output);
}

void write(BinaryWriter& bin, const float& output)
{
  bin.write(output);
}

void write(BinaryWriter& bin, const double& output)
{
  bin.write(output);
}

void write(BinaryWriter& bin, const bool& output)
{
  bin.write(output);
}

// Different bindings for write functions
BinaryWriter& operator<<(BinaryWriter& bin, const std::string& output)
{
  write(bin, output);
  return bin;
}

BinaryWriter& operator<<(BinaryWriter& bin, const int& output)
{
  write(bin, output);
  return bin;
}

BinaryWriter& operator<<(BinaryWriter& bin, const unsigned int& output)
{
  write(bin, output);
  return bin;
}

BinaryWriter& operator<<(BinaryWriter& bin, const short int& output)
{
  write(bin, output);
  return bin;
}

BinaryWriter& operator<<(BinaryWriter& bin, const unsigned short int& output)
{
  write(bin, output);
  return bin;
}

BinaryWriter& operator<<(BinaryWriter& bin, const long int& output)
{
  write(bin, output);
  return bin;
}

BinaryWriter& operator<<(BinaryWriter& bin, const unsigned long int& output)
{
  write(bin, output);
  return bin;
}

BinaryWriter& operator<<(BinaryWriter& bin, const float& output)
{
  write(bin, output);
  return bin;
}

BinaryWriter& operator<<(BinaryWriter& bin, const double& output)
{
  write(bin, output);
  return bin;
}

BinaryWriter& operator<<(BinaryWriter& bin, const bool& output)
{
  write(bin, output);
  return bin;
}

void BinaryWriter::write(const string& output)
{
  size_t n = output.length();
  writeArray(output.c_str(), sizeof(char), n);
  write('\n');   // Convention is to write a line terminator
}

void BinaryWriter::write(const char& output) 
{
  writePrimitive<char>(output);
}

void BinaryWriter::write(const int& output) 
{
  writePrimitive<int>(output);
}

void BinaryWriter::write(const unsigned int& output)
{
  writePrimitive<unsigned int>(output);
}

void BinaryWriter::write(const short int& output)
{
  writePrimitive<short int>(output);
}

void BinaryWriter::write(const unsigned short int& output)
{
  writePrimitive<unsigned short int>(output);
}

void BinaryWriter::write(const long int& output)
{
  writePrimitive<long int>(output);
}

void BinaryWriter::write(const unsigned long int& output)
{
  writePrimitive<unsigned long int>(output);
}

void BinaryWriter::write(const float& output)
{
  writePrimitive<float>(output);
}

void BinaryWriter::write(const double& output)
{
  writePrimitive<double>(output);
}

void BinaryWriter::write(const bool& output)
{
  writePrimitive<bool>(output);
}

template< typename T>
void BinaryWriter::writePrimitive(const T& output)
{
  writeArray((const char*)&output, sizeof(T), 1);
}

void BinaryWriter::writeArray(const char* output, size_t size, size_t nmemb)
{
  if (Layout::primaryNode())
  {
    if (QDPUtil::big_endian())
    {
      /* big-endian */
      /* Write */
      getOstream().write(output, size*nmemb);
    }
    else
    {
      /* little-endian */
      /* Swap and write and swap */
      QDPUtil::byte_swap(const_cast<char *>(output), size, nmemb);
      getOstream().write(output, size*nmemb);
      QDPUtil::byte_swap(const_cast<char *>(output), size, nmemb);
    }
  }
}


QDP_END_NAMESPACE();
