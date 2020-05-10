
#include <string>
#include <exception>
#include <stdexcept>
#include <vector>
#include <memory>

using namespace std;

#ifndef _app_ctx_hpp
#define _app_ctx_hpp

namespace raspserver {

class app_ctx {
	private:
		app_ctx(){}
		~app_ctx(){}

	public:
		static app_ctx& get_instance() {
			static app_ctx instance;
			return instance;
		}
};

}

#endif /*_app_ctx_hpp*/
