include "njsio":io
include "njscommon":common

sp main
	this << 5 $common.push_back; *this;
	this << $common.reverse; *this;
	this << $io.print; *this;
	X << "" $io.println_pop; *X;
	A << 3 2 1;
	B << 5 4;
	Y << &B &A $common.concatenate; *Y;
	Y << $io.print; *Y;
	X << "" $io.println_pop; *X;
end
