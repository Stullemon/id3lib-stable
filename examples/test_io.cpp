// $Id: test_io.cpp,v 1.1 2000/10/08 20:32:52 eldamitri Exp $

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <id3/debug.h>

#include <id3/readers.h>
#include <id3/reader_decorators.h>
#include <id3/writers.h>
#include <id3/writer_decorators.h>
#include <id3/writers_compressed.h>
#include <id3/readers_compressed.h>

using namespace dami;

int
main(size_t argc, const char** argv)
{
  ID3D_INIT_DOUT();
  ID3D_INIT_WARNING();
  ID3D_INIT_NOTICE();

  ID3_IStreamReader isr(cin);
  BString orig;

  {
    io::BinaryReader br(isr);
    
    orig = br.readBinary();
    
  }
  cout << "input size:    " << orig.size() << endl;
  
  cout << endl;

  cout << "=== Testing Synchronization ===" << endl;
  BString synced;

  {
    io::StringReader sr(orig);
    io::UnsyncedReader ur(sr);
    io::BinaryReader br(ur);

    synced = br.readBinary();
  }

  cout << "synced size:   " << synced.size() << endl;

  BString unsynced;

  {
    io::StringWriter sw(unsynced);
    io::UnsyncedWriter uw(sw);

    uw.writeChars((unsigned const char*)synced.data(), synced.size());

  }
  
  cout << "unsynced size: " << unsynced.size() << endl;

  BString resynced;

  {
    io::StringReader sr(unsynced);
    io::UnsyncedReader ur(sr);
    io::BinaryReader br(ur);

    resynced = br.readBinary();
  }

  cout << "resynced size: " << resynced.size() << endl;

  if (unsynced == orig)
  {
    cout << "orig == unsynced" << endl;
  }
  else
  {
    cout << "orig != unsynced" << endl;
  }
  if (synced == resynced)
  {
    cout << "synced == resynced" << endl;
  }
  else
  {
    cout << "synced != resynced" << endl;
  }

  cout << endl;
  cout << "=== Testing Trailing Spaces ===" << endl;

  String text;
  {
    io::StringWriter sw(text);
    io::TrailingSpacesWriter tsw(sw);
    tsw.writeString("hello, world", 50);
  }

  cout << "new text  = \"" << text << "\"" << endl;

  String origText;

  {
    io::StringReader sr(text);
    io::TrailingSpacesReader tsr(sr);
    origText = tsr.readString(100);
  }

  cout << "orig text = \"" << origText << "\"" << endl;

  cout << endl;
  cout << "=== Testing Binary Numbers ===" << endl;

  String number;

  {
    io::StringWriter sw(number);
    io::BinaryNumberWriter bnw(sw);
    bnw.writeNumber(1234567890, 4);
  }

  cout << "binary number:";
  for (size_t i = 0; i < number.size(); ++i)
  {
    cout << " 0x" << hex << (size_t) (0xFF & number[i]) << dec;
  }
  cout << endl;

  size_t val;

  {
    io::StringReader sr(number);
    io::BinaryNumberReader bnr(sr);
    val = bnr.readNumber(4);
  }

  cout << "orig number: " << val << endl;

  cout << endl;
  cout << "=== Testing Compression ===" << endl;

  String compressed;
  size_t origSize = orig.size();
  cout << "origSize = " << origSize << endl;
  

  {
    io::StringWriter sw(compressed);
    io::CompressedWriter cw(sw);
    cw.writeChars(orig.data(), orig.size());
    cw.flush();
    if (origSize != cw.getOrigSize())
    {
      origSize = cw.getOrigSize();
      cout << "cw.getOrigSize() = " << origSize << endl;
    }
  }

  cout << "compressed size = " << compressed.size() << endl;

  String uncompressed;
  
  if (origSize == 0)
  {
    cout << "no compression" << endl;
  }
  else
  {
    io::StringReader sr(compressed);
    io::CompressedReader cr(sr, origSize);
    io::BinaryReader br(cr);

    uncompressed = br.readBinary();
    cout << "uncompressed size = " << uncompressed.size() << endl;
  }

  if (uncompressed.size() == orig.size())
  {
    if (uncompressed == orig)
    {
      cout << "orig == uncompressed" << endl;
    }
    else
    {
      cout << "orig != uncompressed" << endl;
    }
  }


  return 0;
}
