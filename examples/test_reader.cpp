// $Id: test_reader.cpp,v 1.1 2000/09/27 04:19:28 eldamitri Exp $

#include <id3/readers.h>
#include <id3/reader_decorators.h>
#include <string>

int
main(size_t argc, const char** argv)
{
  id3::IStreamReader iosreader(cin);
  id3::UnsyncedReader unsync(iosreader);
  id3::WindowedReader reader(unsync);
  reader.setWindow(50, 50);
  reader.setCur(reader.getBeg());

  ID3_Reader::int_type ch;
  string str;
  while ((ch = reader.readChar()) != ID3_Reader::END_OF_READER)
  {
    str += ch;
  }
  cout << endl << endl;

  id3::MemoryReader mr(str.data(), str.size());
  while ((ch = mr.readChar()) != ID3_Reader::END_OF_READER)
  {
    cout << (char) ch;
  }
  cout << endl;

  return 0;
}
