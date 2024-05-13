#include "file.h"

using namespace std;

namespace bitcask {

File::File( string &path, OpenOptions options ) {
    open( path, options );
}
File::File( const char *path, OpenOptions options ) {
    open( path, options );
}
File::File( string_view path, OpenOptions options ) {
    open( path, options );
}

Result< i32, string_view > OpenOptions::getMode() {
    if ( verify() == false ) {
        return Err( string_view( "Invalid mode" ) );
    }
    i32 mode = 0;
    if ( ReadMode ) mode |= std::ios::in;
    if ( WriteMode ) mode |= std::ios::out;
    if ( AppendMode ) mode |= std::ios::app;
    if ( TruncateMode ) mode |= std::ios::trunc;
    if ( AteMode ) mode |= std::ios::ate;
    if ( BinaryMode ) mode |= std::ios::binary;
    return Ok( mode );
}

bool OpenOptions::verify() {
    if ( AppendMode && TruncateMode ) {
        return false;
    }
    return true;
}

} // namespace bitcask