#include <assert.h>
#include <uuid/uuid.h>
#include <stdio.h>

int 
main()
{
	char u1str[36+1];
	char u2str[36+1];

	uuid_t u1;
	uuid_t u2;
	uuid_generate(u1);
	uuid_generate_random(u2);
	uuid_unparse(u1, u1str);
	u1str[36] = 0;
	uuid_unparse(u2, u2str);
	u2str[36] = 0;

	fprintf(stderr, "uuid_generate(): %s\n", u1str);
	fprintf(stderr, "uuid_generate_random(): %s\n", u2str);

	uuid_t u3;
	uuid_parse(u1str, u3);

	uuid_t u4;
	uuid_parse(u2str, u4);
	// should succeed
	assert(uuid_compare(u1, u3) == 0);
	// should fail
	assert(uuid_compare(u1, u4) == 0);

}
