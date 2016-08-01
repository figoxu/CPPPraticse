#ifndef _MARTAIN_H_
#define _MARTAIN_H_

class Martain
{
	public:
		Martain();
		~Martain();

		void fight();
		void hide();
		static int getCount();
	private:
		static int martainCount; //定义类的静态成员 
		int m_bloodValue;
};

#endif
