#ifndef __EXCEPTION_HPP_____________________20041221154040__
#define __EXCEPTION_HPP_____________________20041221154040__


namespace hpgns
{
	namespace error
	{
		//typedef std::exception exception;
		class exception
		{
		private:
			char *msg;
			int ec;
		public:
			exception() { ec = 0; }
			exception(char *m) { msg = m; }
			exception(int e_code) { ec = e_code; }
			char * message() { return msg; }
			int code() { return ec; }
		};
	}
}


#endif//__EXCEPTION_HPP_____________________20041221154040__
