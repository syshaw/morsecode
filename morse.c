#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

struct MorseMap{
	char value;
	char morsecode[8];
} morse[] = {
	{'a',	".-"},		{'b',	"-..."},	{'c',	"-.-."},	{'d',	"-.."},
	{'e',	"."},		{'f',	"..-."},	{'g',	"--."},		{'h',	"...."},
	{'i',	".."},		{'j',	".---"},	{'k',	"-.-"},		{'l',	".-.."},
	{'m',	"--"},		{'n',	"-."},		{'o',	"---"},		{'p',	".--."},
	{'q',	"--.-"},	{'r',	".-."},		{'s',	"..."},		{'t',	"-"},
	{'u',	"..-"},		{'v',	"...-"},	{'w',	".--"},		{'x',	"-..-"},
	{'y',	"-.--"},	{'z',	"--.."},	{'1',	".----"},	{'2',	"..---"},
	{'3',	"...--"},	{'4',	"....-"},	{'5',	"....."},	{'6',	"-...."},
	{'7',	"--..."},	{'8',	"---.."},	{'9',	"----."},	{'0',	"-----"},
	{'.',	".-.-.-"},	{':',	"---..."},	{',',	"--..--"},	{';',	"-.-.-."},
	{'?',	"..--.."},	{'=',	"-...-"},	{'\'',	".----."},	{'/',	"-..-."},
	{'!',	"-.-.--"},	{'-',	"-....-"},	{'_',	"..--.-"},	{'"',	".-..-."},
	{'(',	"-.--."},	{')',	"-.--.-"},	{'$',	"...-..-"},	{'&',	".-..."},
	{'@',	".--.-."},	{'+',	".-.-."}
	
};

int left_match(char ch)
{
	if (ch == '.' || ch == '*')	return 0;
	return -1;
}

int right_match(char ch)
{
	if (ch == '-' || ch == '_')	return 0;
	return -1;
}

struct TreeNode{
	struct TreeNode *lchild;
	struct TreeNode	*rchild;
	char data;
};

int create_tree(struct TreeNode *root, struct MorseMap *mm)
{
	char *tmp = mm->morsecode;
	struct TreeNode *curnode = root;
	while (*tmp) {
		if (!left_match(*tmp)) {
			if (curnode->lchild) {
				curnode = curnode->lchild;
			} else {
				struct TreeNode *tmpnode = NULL;
				tmpnode = malloc(sizeof(struct TreeNode));
				if (!tmpnode) return -1;
				curnode->lchild = tmpnode;
				curnode = tmpnode;
			}
		} else if (!right_match(*tmp)) {
			if (curnode->rchild) {
				curnode = curnode->rchild;
			} else {
				struct TreeNode *tmpnode = NULL;
				tmpnode = malloc(sizeof(struct TreeNode));
				if (!tmpnode) return -1;
				curnode->rchild = tmpnode;
				curnode = tmpnode;
			}
		}
		tmp++;
	}
	curnode->data = mm->value;
	return 0;
}


struct TreeNode* init_morse()
{
	int i;

	struct TreeNode *root = NULL;
	root = malloc(sizeof(struct TreeNode));
	if (!root) return NULL;
	for (i = 0; i < sizeof(morse)/sizeof(morse[0]); i++) {
		create_tree(root, &morse[i]);
	}
	return root;
}

void free_morse(struct TreeNode* root)
{
	if (!root) return;
	if (root->lchild) {
		free_morse(root->lchild);
	}
	if (root->rchild) {
		free_morse(root->rchild);
	}
	free(root);
}

int find_value(struct TreeNode* root, char *morse, char *value)
{
	char *tmp = morse;
	struct TreeNode *curnode = root;
	while (*tmp) {
		if (!left_match(*tmp)) {
			curnode = (curnode && curnode->lchild)?curnode->lchild:NULL;
		} else if (!right_match(*tmp)) {
			curnode = (curnode && curnode->rchild)?curnode->rchild:NULL;
		}
		if (!curnode) break;
		tmp++;
	}
	*value = curnode?curnode->data:'\0';
	return *value?1:0;
}

int find_morse(struct TreeNode* root, char value, char *morse)
{
	int ret = 0;
	struct TreeNode *curnode = root;
	if (curnode->data == value) return 1;
	if (curnode->lchild) {
		ret = find_morse(curnode->lchild, value, morse+1);
		if (ret) *morse = '.';
	}
	if (!ret && curnode->rchild) {
		ret = find_morse(curnode->rchild, value, morse+1);
		if (ret) *morse = '-'; 
	}
	return ret;
}

int g_e_flag;char g_e_data[512];
int g_d_flag;char g_d_data[512];
void parse_commandline(int argc, char *argv[])
{
	int option = 0;
	while ((option = getopt(argc, argv, "e:d:")) != EOF) {
		switch (option) {
		case 'e':
			g_e_flag = 1;
			snprintf(g_e_data, sizeof(g_e_data), "%s", optarg);
			break;
		case 'd':
			g_d_flag = 1;
			snprintf(g_d_data, sizeof(g_d_data), "%s", optarg);
			break;
		default:
			printf("option:\n\t-e parameter\tmorse code encrypt\n\t-d parameter\tmorse code dencrypt\n");
			exit(-1);
		}
	}
}

int main(int argc, char **argv)
{
	char *token = NULL;

	parse_commandline(argc, argv);
	struct TreeNode* root = init_morse();
	if (g_d_flag) {
		char *tmp = g_d_data;
		printf("%s\n", tmp);
		while ((token = strsep(&tmp, "/")) != NULL) {
			char value = 0;
			if (strlen(token) <= 0) continue;
			if (find_value(root, token, &value)) {
				printf("%c", value);
			} else {
				printf("[error(%s)]", token);
			}
		}
	}

	if (g_e_flag) {
		char *tmp = g_e_data;
		printf("\n\n%s\n", tmp);
		for (int i = 0; i < strlen(tmp); i++) {
			char morse[8] = {0};
			if(find_morse(root, tmp[i], morse)) printf("%s", morse);
			else printf("[error(%c)]", tmp[i]);
			printf("%s", ((i + 1) == strlen(tmp)) ? "":" ");
		}
	}
	free_morse(root);
	printf("\ndone!\n");
}
