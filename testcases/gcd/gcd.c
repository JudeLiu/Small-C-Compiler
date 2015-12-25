#include <stdio.h>
int a,b;

int gcd(int x,int y)
{
	if(!y)
		return x;
	return gcd(y,x%y);
}

int main()
{
	scanf("%d",&a);
	scanf("%d",&b);
	printf("%d",gcd(a,b));
	return 0;
}
