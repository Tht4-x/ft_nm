/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajeon <chajeon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 08:54:05 by dancel            #+#    #+#             */
/*   Updated: 2025/02/14 17:11:17 by chajeon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	unsigned char	*t;
	unsigned char	d;

	t = (unsigned char *)s;
	d = (unsigned char)c;
	while (n > 0)
	{
		if (*t == d)
			return (t);
		t++;
		n--;
	}
	return (NULL);
}
