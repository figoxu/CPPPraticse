#ifndef _STUDENT_H_
#define _STUDENT_H_

class Student{
public:
	explicit Student(int id=0);
	~Student();

	inline int getId() const;

	void setId(int id){
		m_id = id;
	}

	int getScore() const{
		return m_id;
	}

	void setScore(int score){
		m_score = score;
	}

private:
	int m_id;
	int m_score;
};

int Student::getId() const
{
	return m_id;
}

#endif
