#ifndef LIBFT_HPP
# define LIBFT_HPP

#include <string>
#include <vector>
#include <iostream>

void	ft_split(const std::string &target, const std::string &sep, std::vector<std::string> &infos);
void    *ft_memset(void *s, int c, size_t n);
#endif