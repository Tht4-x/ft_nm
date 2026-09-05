/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strstr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajeon <chajeon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 17:06:38 by dancel            #+#    #+#             */
/*   Updated: 2025/02/14 17:09:58 by chajeon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strstr(const char *big, const char *little)
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
	while (b[i])
	{
		j = 0;
		while (b[i + j] == l[j])
		{
			j++;
			if (j == ft_strlen(l))
				return (&b[i]);
		}
		i++;
	}
	return (NULL);
}
