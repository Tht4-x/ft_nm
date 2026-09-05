/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajeon <chajeon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 10:15:49 by dancel            #+#    #+#             */
/*   Updated: 2025/02/14 17:10:08 by chajeon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	char		*b;
	char		*l;
	size_t		i;
	size_t		j;

	b = (char *)big;
	l = (char *)little;
	i = 0;
	if (l[0] == '\0')
		return (b);
	while (b[i] && i < len)
	{
		j = 0;
		while (b[i + j] == l[j] && i + j < len)
		{
			j++;
			if (j == ft_strlen(l))
				return (&b[i]);
		}
		i++;
	}
	return (NULL);
}
