#ifndef FD_BASE_HPP
# define FD_BASE_HPP

typedef enum			t_fd_type
{
	FD_SERVER,
	FD_CLIENT,
	FD_RESOURCE
}						e_fd_type;

class FdBase
{
	protected:
		int m_fd;
		e_fd_type m_fd_type;

	public:
		FdBase();
		FdBase(const FdBase &other);
		virtual ~FdBase();
		FdBase &operator=(const FdBase &other);

		int getFd(void) const;
		e_fd_type getFdType(void) const;

		void setFd(int fd);
		void setFdType(e_fd_type fd_type);
};

#endif