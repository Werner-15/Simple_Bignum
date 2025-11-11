#include <stdint.h>
#include <stdlib.h>

/*5<LSIZE<32767*/
#define LSIZE 200


/**
 *	@brief	This structure defines an integer which can be extremely large.
 *  @detail	num[0]=Lowest digit,num[1]=2nd lowest,num[2]=3rd lowest,...
 * 
 * array index:	0,1,2,3,4,...,max
 * weigh:low->high
 * Unused high digits should be all 0.
 * 
 */
typedef struct LargeInt{
	uint8_t isNegative;
	uint8_t num[LSIZE];
	
} LargeInt;


typedef enum LargeInt_CheckLegal_Result{
	LargeInt_CheckLegal_Result_OK,
	LargeInt_CheckLegal_Result_Fail
} LargeInt_CheckLegal_Result;

/**
 * @brief It helps to check if x is okay.
 * 
 * @param x:The num to identify.
 * 
 * @return @ref LargeInt_CheckLegal_Result
 */
LargeInt_CheckLegal_Result LargeInt_CheckLegal(LargeInt *x){
	if(x==NULL|| 
		(x->isNegative!=0 && x->isNegative!=1)){
		
		return LargeInt_CheckLegal_Result_Fail;
	}
	uint16_t i=0;
	for(i=0;i<LSIZE;i++){
		if(x->num[i]>=10){
			
			return LargeInt_CheckLegal_Result_Fail;
		}
	}
	return LargeInt_CheckLegal_Result_OK;
}

/**
 *@brief	This function turns a long long into a largeInt
 *@param	LargeInt *returnPlace: the place to return the output.
 *			You should input a variable's address as a parameter.
 *@param	int64_t input: the input long long int.
*/
void LargeInt_Long2Large(LargeInt *returnPlace,int64_t input){
	if(returnPlace==NULL){
		return;
	}
	
	LargeInt output;
	output.isNegative = (input<0)?1:0;
	/*i is here to decide the index.*/
	uint16_t i=0;
	uint64_t abs_input = input>=0 ? input:(-input);
	
	
	for(i=0;abs_input!=0;i++,abs_input/=10){
		output.num[i] = abs_input%10;
		
	}
	/*Fill the rest.*/
	for(;i<LSIZE;i++){
		output.num[i]=0;
	}
	
	/*Output.*/
	returnPlace->isNegative = output.isNegative;
	for(i=0;i<LSIZE;i++){
		returnPlace->num[i] = output.num[i];
	}
	
	
}



/**
 * @defgroup	LargeCompareResult
 * @brief	The type is used as return values of compare function.
 * 			
 */
typedef enum LargeCompareResult{
	LargeCompareResult_SAME=0,
	LargeCompareResult_A_BIG=1,
	LargeCompareResult_A_SMALL=-1,
	LargeCompareResult_Error_Param=2,
	LargeCompareResult_Error_Conflict
} LargeCompareResult;

/**
 *@brief	It helps compare the two large ints without changing either.
 *@param	LargeInt *a,LargeInt *b:Two large ints to compare.
 *@return	LargeCompareResult:see @ref	LargeCompareResult.
 */
LargeCompareResult LargeInt_Compare(LargeInt *a,LargeInt *b){
	int16_t i = LSIZE-1;
	uint8_t reverse_result = 0;
	LargeCompareResult res;
	
	
	if(LargeInt_CheckLegal(a)==LargeInt_CheckLegal_Result_Fail ||
	LargeInt_CheckLegal(b)==LargeInt_CheckLegal_Result_Fail){
		
		return LargeCompareResult_Error_Param;
	}
	
	/*Compare the sign.*/
	if(a->isNegative==1){
		if(b->isNegative==0){
			return LargeCompareResult_A_SMALL;
		}
		else{
			reverse_result=1;
		}
	}
	else{
		if(b->isNegative==1){
			return LargeCompareResult_A_BIG;
		}
	}
	
	res = LargeCompareResult_SAME;
	/*Compare each digit.*/
	for(i = LSIZE-1;i>=0;i--){
		if(a->num[i]!=b->num[i]){
			
			if(a->num[i]>b->num[i]){
				res = LargeCompareResult_A_BIG;
			}
			else{
				res = LargeCompareResult_A_SMALL;
			}
			break;
		}
		
	}
	
	
	
	/*Reverse.*/
	if(reverse_result==1){
		switch(res){
		case LargeCompareResult_A_BIG:
			res = LargeCompareResult_A_SMALL;
			break;
		case LargeCompareResult_A_SMALL:
			res = LargeCompareResult_A_BIG;
			break;
		case LargeCompareResult_SAME:
			break;
		default:
			res = LargeCompareResult_Error_Conflict;
			break;
		}
	}
	return res;
}

typedef enum LargeInt_Add_Results{
	LargeInt_Add_Results_OK,
	LargeInt_Add_Results_Error_Param,
	LargeInt_Add_Results_Overflow,
	LargeInt_Add_Results_Program_Error
} LargeInt_Add_Results;

/**
 *@brief	This function ignores the two numbers' sign,
 *			only adding their numbers together.
 *@param	a,b:The two LargeInts to add.
 *@param 	returnPlace:The place to return.
 */
static LargeInt_Add_Results LargeInt_Add_Unsigned(LargeInt *returnPlace,LargeInt *a,LargeInt *b){
	int16_t i=0;
	uint8_t carry = 0;
	
	/*Check.*/
	if(LargeInt_CheckLegal(a)==LargeInt_CheckLegal_Result_Fail ||
	LargeInt_CheckLegal(b)==LargeInt_CheckLegal_Result_Fail	||
	returnPlace==NULL){
		return LargeInt_Add_Results_Error_Param;
	}
	
	/*Default sign.*/
	returnPlace->isNegative = 0;
	
	/*Add the digits.*/
	carry = 0;
	for(i=0;i<LSIZE;i++){
		returnPlace->num[i] = a->num[i] + b->num[i] + carry;
		carry = returnPlace->num[i]/10;
		returnPlace->num[i] %= 10;
	}
	
	
	/*Return.*/
	if(carry!=0){
		return LargeInt_Add_Results_Overflow;
	}
	return LargeInt_Add_Results_OK;
}



typedef enum LargeInt_Sub_Results{
	LargeInt_Sub_Results_OK,
	LargeInt_Sub_Results_Error_Param,
	LargeInt_Sub_Results_Underflow,
	LargeInt_Sub_Results_Program_Error
} LargeInt_Sub_Results;

/**
 * @brief Unsigned subtraction.
 * @param returnPlace the place to return.
 * @param a Number to minus.
 * @param b The sub value.
 * @note 1. ASSERT a>=b !!!
 *       2. It also ignores the sign.
 */
static LargeInt_Sub_Results LargeInt_Sub_Unsigned(LargeInt *returnPlace, LargeInt *a, LargeInt *b) {
	int16_t i = 0;
	uint8_t borrow = 0;
	/*Temporary difference.*/
	int16_t diff=0;
	
	/*Check.*/
	if (LargeInt_CheckLegal(a) != LargeInt_CheckLegal_Result_OK ||
		LargeInt_CheckLegal(b) != LargeInt_CheckLegal_Result_OK ||
		returnPlace == NULL) {
		return LargeInt_Sub_Results_Error_Param;
	}
	
	/*Default sign.*/
	returnPlace->isNegative = 0;
	
	/* Minus each digit.*/
	for (i = 0; i < LSIZE; i++) {
		diff = (int16_t)a->num[i] - (int16_t)b->num[i] - (int16_t)borrow;
		borrow = 0;
		
		while(diff<0) {
			diff+=10;
			borrow++;
		}
		
		/*Output.*/
		returnPlace->num[i] = (uint8_t)diff;
	}
	
	/*Return.*/
	if (borrow != 0) {
		return LargeInt_Sub_Results_Underflow;
	}
	
	return LargeInt_Sub_Results_OK;
}


/**
 * @brief It adds a and b together,considering the sign.
 * 
 * @param a Number a.
 * @param b Number b.
 * @param returnPlace	The place to return. 
 * 
 * @return LargeInt_Add_Results the result.
 */
LargeInt_Add_Results LargeInt_Add(LargeInt *returnPlace,LargeInt *a,LargeInt *b){
	LargeInt_Add_Results st;
	LargeInt_Sub_Results sst;
	LargeCompareResult com;
	LargeInt temp;
	uint16_t i=0;
	
	/*Check.*/
	if (LargeInt_CheckLegal(a) != LargeInt_CheckLegal_Result_OK ||
		LargeInt_CheckLegal(b) != LargeInt_CheckLegal_Result_OK ||
		returnPlace == NULL) {
		return LargeInt_Add_Results_Error_Param;
	}
	
	/*Consider the 4 conditions.*/
	if(a->isNegative==0){
		if(b->isNegative==0){
			st=LargeInt_Add_Unsigned(returnPlace,a,b);
			returnPlace->isNegative=0;
			return st;
		}
		else if(b->isNegative==1){
			temp.isNegative=0;
			for(i=0;i<LSIZE;i++){
				temp.num[i]=b->num[i];
			}
			com=LargeInt_Compare(a,&temp);
			
			
			/*Act according to different sizes of a and b.*/
			switch(com){
			case LargeCompareResult_A_BIG:
				/*a>b,a-abs(b)*/
				sst = LargeInt_Sub_Unsigned(returnPlace,a,b);
				returnPlace->isNegative=0;
				break;
			case LargeCompareResult_A_SMALL:
				/*a<b,a-abs(b)=-(abs(b)-a)*/
				sst = LargeInt_Sub_Unsigned(returnPlace,b,a);
				returnPlace->isNegative=1;
				break;
			case LargeCompareResult_SAME:
				for(i=0;i<LSIZE;i++){
					returnPlace->num[i]=0;
				}
				returnPlace->isNegative=0;
				sst = LargeInt_Sub_Results_OK;
				break;
			default:
				return LargeInt_Add_Results_Program_Error;
			}
			
			/*Deal with the state.*/
			switch(sst){
			case LargeInt_Sub_Results_OK:
				return LargeInt_Add_Results_OK;
			case LargeInt_Sub_Results_Underflow:
				return LargeInt_Add_Results_Overflow;
			case LargeInt_Sub_Results_Error_Param:
				return LargeInt_Add_Results_Error_Param;
			case LargeInt_Sub_Results_Program_Error:
				return LargeInt_Add_Results_Program_Error;
			default:
				return LargeInt_Add_Results_Program_Error;
			}
			return LargeInt_Add_Results_Program_Error;
			
		}
		else{
			return LargeInt_Add_Results_Error_Param;
		}
	}
	
	else if(a->isNegative==1){
		if(b->isNegative==1){
			st=LargeInt_Add_Unsigned(returnPlace,a,b);
			returnPlace->isNegative=1;
			return st;
		}
		else if(b->isNegative==0){
			temp.isNegative=0;
			for(i=0;i<LSIZE;i++){
				temp.num[i]=a->num[i];
			}
			
			com=LargeInt_Compare(&temp,b);
			
			
			/*Act according to different sizes of a and b.*/
			switch(com){
			case LargeCompareResult_A_SMALL:
				/*a<b,b-abs(a)*/
				sst = LargeInt_Sub_Unsigned(returnPlace,b,a);
				returnPlace->isNegative=0;
				break;
			case LargeCompareResult_A_BIG:
				/*a>b,b-abs(a) = -(abs(a)-b)*/
				sst = LargeInt_Sub_Unsigned(returnPlace,a,b);
				returnPlace->isNegative=1;
				break;
			case LargeCompareResult_SAME:
				for(i=0;i<LSIZE;i++){
					returnPlace->num[i]=0;
				}
				returnPlace->isNegative=0;
				sst = LargeInt_Sub_Results_OK;
				break;
			default:
				return LargeInt_Add_Results_Program_Error;
			}
			
			/*Deal with the state.*/
			switch(sst){
			case LargeInt_Sub_Results_OK:
				return LargeInt_Add_Results_OK;
			case LargeInt_Sub_Results_Underflow:
				return LargeInt_Add_Results_Overflow;
			case LargeInt_Sub_Results_Error_Param:
				return LargeInt_Add_Results_Error_Param;
			case LargeInt_Sub_Results_Program_Error:
				return LargeInt_Add_Results_Program_Error;
			default:
				return LargeInt_Add_Results_Program_Error;
			}
			return LargeInt_Add_Results_Program_Error;
			
		}
		else{
			return LargeInt_Add_Results_Error_Param;
		}
	}
	else{
		return LargeInt_Add_Results_Error_Param;
	}
	
	
}



/**
 * @brief It calculates a-b,considering the sign.
 * 
 * @param a Number a.
 * @param b Number b.
 * @param returnPlace	The place to return. 
 * 
 * @return LargeInt_Add_Results the result.
 */
LargeInt_Sub_Results LargeInt_Sub(LargeInt *returnPlace,LargeInt *a,LargeInt *b){
	LargeInt_Sub_Results ret;
	LargeInt_Add_Results ad;
	LargeInt neg_b;
	uint16_t i;
	
	/*Check.*/
	if (LargeInt_CheckLegal(a) != LargeInt_CheckLegal_Result_OK ||
		LargeInt_CheckLegal(b) != LargeInt_CheckLegal_Result_OK ||
		returnPlace == NULL) {
		return LargeInt_Sub_Results_Error_Param;
	}
	
	neg_b.isNegative = !(b->isNegative);
	for(i=0;i<LSIZE;i++){
		neg_b.num[i] = b->num[i];
	}
	
	ad=LargeInt_Add(returnPlace,a,&neg_b);
	
	/*Deal with the state.*/
	switch(ad){
	case LargeInt_Add_Results_OK:
		return LargeInt_Sub_Results_OK;
	case LargeInt_Add_Results_Overflow:
		return LargeInt_Sub_Results_Underflow;
	case LargeInt_Add_Results_Error_Param:
		return LargeInt_Sub_Results_Error_Param;
	case LargeInt_Add_Results_Program_Error:
		return LargeInt_Sub_Results_Program_Error;
	default:
		return LargeInt_Sub_Results_Program_Error;
	}
	return LargeInt_Sub_Results_Program_Error;
	
}




typedef enum LargeInt_Multiply_Results{
	LargeInt_Multiply_Results_OK,
	LargeInt_Multiply_Results_Error_Param,
	LargeInt_Multiply_Results_Overflow,
	LargeInt_Multiply_Results_Program_Error
} LargeInt_Multiply_Results;


/**
 *@brief	This function tries to multiply two large numbers,considering signs.
 *@param	returnPlace:The place to return the results.
 *@param	a,b:The numbers to multiply.
 *@return	LargeInt_Multiply_Results.
*/
LargeInt_Multiply_Results  \
LargeInt_Multiply(LargeInt *returnPlace,LargeInt *a,LargeInt *b){
	LargeInt result;
	uint16_t alen=0,blen=0;
	int16_t i,j;
	
	
	/*Check.*/
	if (LargeInt_CheckLegal(a) != LargeInt_CheckLegal_Result_OK ||
		LargeInt_CheckLegal(b) != LargeInt_CheckLegal_Result_OK ||
		returnPlace == NULL) {
		return LargeInt_Multiply_Results_Error_Param;
	}
	
	
	/*Count how many digits there are in each number.*/
	alen=1,blen=1;
	for(i=LSIZE-1;i>=0;i--){
		if(a->num[i]!=0){
			alen=i+1;
			break;
		}
	}
	for(i=LSIZE-1;i>=0;i--){
		if(b->num[i]!=0){
			blen=i+1;
			break;
		}
	}
	
	/*For calculation a(m digits) times b(n digits),
	the result's number of digits should be 
	either m+n-1 or m+n.
	This can be used as a proof of overflow.*/
	if(alen+blen-1>LSIZE){
		return LargeInt_Multiply_Results_Overflow;
	}
	
	/*For convenience,we should make sure alen>=blen,
	otherwise we swap the two numbers.*/
	if(alen<blen){
		return LargeInt_Multiply(returnPlace,b,a);
	}
	
	
	/*
	Now judge the sign.
	sign equal ->0,
	different ->1,
	consider XOR calculation.
	*/
	result.isNegative = a->isNegative ^ b->isNegative;
	result.isNegative &=0x01;/*Clear up other bits.*/
	
	/*Clear result bar.*/
	for(i=0;i<LSIZE;i++){
		result.num[i]=0;
	}
	
	/*
	Now operate the two absolute values' result,
	which is the main part.
	*/
	i=0;j=0;
	for(j=0;j<blen;j++){
		if(b->num[j]==0){
			continue;
		}
		
		for(i=0;i<alen;i++){
			result.num[i+j] += (a->num[i]) * (b->num[j]);
		}
		for(i=0;i<j+alen;i++){
			if(i<LSIZE-1){
				result.num[i+1]+=result.num[i]/10;
			}
			else{
				if(result.num[i]>=10){
					return LargeInt_Multiply_Results_Overflow;
				}
			}
			result.num[i]%=10;
		}
	}
	
	/*return.*/
	for(i=0;i<LSIZE;i++){
		returnPlace->num[i] = result.num[i];
	}
	returnPlace->isNegative = result.isNegative;
	
	return LargeInt_Multiply_Results_OK;
	
	
}






typedef enum LargeInt_Divide_Results{
	LargeInt_Divide_Results_OK,
	LargeInt_Divide_Results_Error_Param,
	LargeInt_Divide_Results_Overflow,
	LargeInt_Divide_Results_Program_Error,
	LargeInt_Divide_Results_Error_Div_0
} LargeInt_Divide_Results;



/**
 * @brief	This function performs a/b,both of which are LargeInts.
 * @param	LargeInt *returnPlace,LargeInt *a,LargeInt *b
 * @note	b!=0
 */
LargeInt_Divide_Results  \
LargeInt_Divide(LargeInt *returnPlace,LargeInt *a,LargeInt *b){
	LargeInt dividend,divisor,remainder,result,buffer;
	uint16_t alen,blen,rlen;
	int16_t i,j,k,l,p;
	LargeCompareResult x;
	
	/*Check.*/
	if (LargeInt_CheckLegal(a) != LargeInt_CheckLegal_Result_OK ||
		LargeInt_CheckLegal(b) != LargeInt_CheckLegal_Result_OK ||
		returnPlace == NULL) {
		return LargeInt_Divide_Results_Error_Param;
	}
	
	/*Div 0 check.*/
	j=1;
	for(i=0;i<LSIZE;i++){
		if(b->num[i]!=0){
			j=0;
			break;
		}
	}
	if(j==1){
		return LargeInt_Divide_Results_Error_Div_0;
	}
	
	/*Count how many digits there are in each number.*/
	alen=1,blen=1;
	for(i=LSIZE-1;i>=0;i--){
		if(a->num[i]!=0){
			alen=i+1;
			break;
		}
	}
	for(i=LSIZE-1;i>=0;i--){
		if(b->num[i]!=0){
			blen=i+1;
			break;
		}
	}
	
	/*
	Now judge the sign.
	sign equal ->0,
	different ->1,
	consider XOR calculation.
	*/
	result.isNegative = a->isNegative ^ b->isNegative;
	result.isNegative &=0x01;/*Clear up other bits.*/
	
	/*Clear result bar and remainder bar,
	set divisor(abs(b)) and dividend(abs(a)).*/
	for(i=0;i<LSIZE;i++){
		result.num[i]=0;
		remainder.num[i]=0;
		divisor.num[i]=b->num[i];
		dividend.num[i]=a->num[i];
	}
	
	dividend.isNegative=0;
	divisor.isNegative=0;
	
	/*Get rid of special conditions.*/
	x=LargeInt_Compare(&dividend,&divisor);
	switch (x) {
	case LargeCompareResult_A_SMALL:
		returnPlace->isNegative=0;
		for(i=0;i<LSIZE;i++){
			returnPlace->num[i]=0;
		}
		return LargeInt_Divide_Results_OK;
		break;
	case LargeCompareResult_SAME:
		returnPlace->num[0]=1;
		for(i=1;i<LSIZE;i++){
			returnPlace->num[i]=0;
		}
		return LargeInt_Divide_Results_OK;
		break;
	case LargeCompareResult_A_BIG:
		break;
	default:
		
		return LargeInt_Divide_Results_Program_Error;
		break;
	}
	
	remainder.isNegative=0;
	divisor.isNegative=0;
	/*Now is the state machine part.*/
	/*j works as the dividend ptr.*/
	j=alen-1;
	/*p is used as result digit indicator.*/
	p=(blen>1)?(alen-blen+1):(alen-1);
	while(1){
		
		/*Remainder length.*/
		rlen=1;
		for(i=LSIZE-1;i>=0;i--){
			
			if(remainder.num[i]!=0){
				rlen=i+1;
				break;
			}
		}
		
		if(rlen!=blen){
			/*
			Move remainder to the higher level.
			It means:
			b=12345
			r=01344
			=>r=13440
			*/
			i=1;
			/*Move digits.*/
			for(i=1;i<=rlen;i++){
				remainder.num[blen-i]=remainder.num[rlen-i];
			}
			for(i=0; i < (blen - rlen); i++){
				remainder.num[i] = 0;
			}
			
			/*Copy the rest digits.*/
			i=j;
			i-=(blen-rlen-1);
			if(i<0){
				i=0;
			}
			for(k=i,l=0 ; k<=j&&l<blen-rlen ; k++,l++){
				remainder.num[l]=dividend.num[k];
			}
				
		}
		else{
			for(k=LSIZE-2;k>=0;k--){
				remainder.num[k+1]=remainder.num[k];
			}
			i=j;
			remainder.num[0]=dividend.num[i];
			
			
		}
		/*
		printf("%d,%d,%d,%d,%d!%d\n",
			remainder.num[4],
			remainder.num[3],
			remainder.num[2],
			remainder.num[1],
			remainder.num[0],p);
		*/
		/*Start dividing.*/
		while(1){
			
			x=LargeInt_Compare(&remainder,&divisor);
			if(x==LargeCompareResult_Error_Conflict ||
				x==LargeCompareResult_Error_Param){
				
				return LargeInt_Divide_Results_Program_Error;
			}
			else if(x==LargeCompareResult_A_SMALL){
				
				break;
			}
			else if(x==LargeCompareResult_A_BIG ||
				x==LargeCompareResult_SAME){
				
				LargeInt_Sub(&buffer,&remainder,&divisor);
				
				for(k=0;k<LSIZE;k++){
					remainder.num[k]=buffer.num[k];
				}
				result.num[p]++;
				
			}
		}
		
		
		j=i-1;
		p--;
		
		if(p<0||p>=LSIZE){
			returnPlace->isNegative = result.isNegative;
			for(i=0;i<LSIZE;i++){
				returnPlace->num[i] = result.num[i];
			}
			return LargeInt_Divide_Results_OK;
		}
		
		
	}
	
}

/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
#include <stdio.h>
#include <time.h>
int main(){
	uint64_t l = 16384299792458;
	uint64_t m = -1145141919810;
	
	uint16_t i = 0,j;
	LargeInt x,y,z;
	
	time_t t1,t2;
	
	LargeInt_Multiply_Results times_res;
	LargeInt_Divide_Results div_res;
	
	LargeInt_Long2Large(&x,l);
	LargeInt_Long2Large(&y,m);
	
	printf("%lld\n",l);
	for(i=0;i<LSIZE;i++){
		printf("%d",x.num[i]);
		
	}
	printf("\n");
	
	printf("%d\n",LargeInt_Compare(&x,&y));
	
	LargeInt_Sub(&z,&x,&y);
	for(i=0;i<LSIZE;i++){
		printf("%d",z.num[i]);
		
	}
	printf("\n");
	
	times_res=LargeInt_Multiply(&z,&x,&y);
	for(i=0;i<LSIZE;i++){
		printf("%d",z.num[i]);
		
	}
	printf("\n");
	printf("%d\n",times_res);
	
	div_res=LargeInt_Divide(&z,&x,&y);
	for(i=0;i<LSIZE;i++){
		printf("%d",z.num[i]);
		
	}
	printf("\n");
	printf("%d\n",div_res);
	
	LargeInt_Multiply(&x,&x,&x);
	t1=clock();
	for(i=1;i<800;i++){
		for(j=1;j<1000;j++){
			LargeInt_Multiply(&z,&x,&y);
		}
	}
	t2=clock();
	printf("time: %d ms\n",t2-t1);
	
	return 0;
}
