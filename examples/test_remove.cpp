// $Id: test_remove.cpp,v 1.2 2000/06/23 00:35:07 eldamitri Exp $

#include <iostream.h>
#include <id3/tag.h>
#include <id3/misc_support.h>

int main( int argc, char *argv[])
{
  try
  {
    ID3_Tag tag;
    ID3_Frame frame;

    tag.Link("test-remove.tag");
    tag.Strip(ID3TT_ALL);
    tag.Clear();

    frame.SetID(ID3FID_TITLE);
    frame.Field(ID3FN_TEXT).Set("Test title");
    tag.AddFrame(frame);

    frame.SetID(ID3FID_COMPOSER);
    frame.Field(ID3FN_TEXT).Set("Test composer");
    tag.AddFrame(frame);

    frame.SetID(ID3FID_BAND);
    frame.Field(ID3FN_TEXT).Set("Test band");
    tag.AddFrame(frame);

    frame.SetID(ID3FID_CONDUCTOR);
    frame.Field(ID3FN_TEXT).Set("Test conductor");
    tag.AddFrame(frame);

    tag.SetPadding(false);
    tag.Update(ID3TT_ID3V2);

    cerr << "removed " << ID3_RemoveArtists(&tag) << " artist frames" << endl;
    tag.Update();
  }   
  catch(const ID3_Error& err)
  {
    cout << endl;
    cout << err.GetErrorFile() << " (" << err.GetErrorLine() << "): "
         << err.GetErrorType() << ": " << err.GetErrorDesc() << endl;
  }

  return 0;
}
