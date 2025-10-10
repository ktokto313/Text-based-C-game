#include <stdio.h>
#include <string.h>

#define MAX 3

typedef struct{
	char name[30];
	int level;
} character;

void displayTeam(character team[], int count){
	int i;
	int (count == 0){
		printf("Doi hinh dang trong");
	}
	else {
		printf("==Doi hinh hien tai==");
		for (i = 0; i < count; i++){
			printf("%d. Ten: %s | level:%d\n", i + 1, team[i].name, team[i].level);
		}
	}
}
void addCharcter(Charcter team[], int*count){
	if(*count >= MAX){
		printf("khong the them! Doi hinh da d? % nhan vat\n", MAX);
		return ;
	}
	Character c;
	printf("nhap nhan vat hien tai:");
	Scanf("%[^\n]", c.game);
	printf("Nhap level nhan vat:");
	Scanf("%d\n", &c.level);
	
	team[*count] = c;
	(*count)++
	printf("Da them nhan vat vao doi hinh!\n");
}
void removeCharacter(Character team[], int *count){
	if(*count == 0){
		printf("khong co nhan vat nao de xoa!\n");
		return;
	}
	displayTeam(team, *count);
	int index;
	printf("chon nhan vat de xoa", *count);
	scanf("%d", &index);
	if (index < 1 || index > *count){
		printf("Lua chon khong hop le!\n");
		return;
	}
	int i;
	for (i = index; i < *count - 1; i++){
		team[i] = team[i + 1];
	}
	(*count)--;
	printf("Da xoa nhan vat khoi doi hinh!\n");
}
void swapCharacter(Character team[], int count){
	if (count < 2){
		printf("Can it nhat 2 nhan vat de doi cho!\n");
		return;
	}
	displayTeam(team, count)
		int a,b;
		printf("Nhap vi tri nhan vat so 1:");
		scanf("%d", &a);
		printf("Nhap vi tri nhan vat so 2:");
		scanf("%d", &b);
		
		if(a < 1 || b < 1 || a > count || b > count){
			printf("Lua chon vuot qua pham vi!\n");
			return;
		}
		Character temp = team[a - 1];
		team[a - 1] = team[b - 1];
		team[b - 1] = temp;
		printf("Da doi vi tri 2 nhan vat!\n");
}
int main(){
	Character team[MAX];
	int count = 0;
	int choice = 0;
	
	do {
		printf("\n======MENU=======\n");
		printf("1. Hien thi doi hinh\n");
		printf("2.Them nhan vat\n");
		printf("3.Xoa nhan vat\n");
		printf("4.Doi nhan vat\n");
		printf("0.Thoat\n");
		printf("Chon: ");
		scanf("%d", &choice);
		
		switch(choice){
			case 1:
				displayTeam(team, count);
				break;
			case 2:
				addCharacter(team, &count);
				break;
			case 3:
				removeCharacter(team, &count);
				break;
			case 4:
				swapCharacter(team, count);
				break;
			case 0:
				printf("Thoat chuong trinh!\n");
				break;
			default:
				printf("Lua chon khong hop le!\n");
		}
	}while(choice != 0);
	
	return 0;
}
