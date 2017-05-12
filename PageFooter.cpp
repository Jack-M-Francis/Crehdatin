#include "PageFooter.h"

void createPageFooter(FcgiData* fcgi, RequestData* data){
	fcgi->out << 
	"</div>"
	"</body>"
	"</html>";
}
