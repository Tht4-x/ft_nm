/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajeon <chajeon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 10:27:18 by dancel            #+#    #+#             */
/*   Updated: 2025/02/14 17:09:53 by chajeon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	begin(char const *s1, char const *set)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (set[j] && s1[i])
	{
		if (s1[i] == set[j])
		{
			i++;
			j = -1;
		}
		j++;
	}
	return (i);
}

static int	end(char const *s1, char const *set)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (set[j] && ft_strlen(s1) - i != 0)
	{
		if (s1[ft_strlen(s1) - 1 - i] == set[j])
		{
			i++;
			j = -1;
		}
		j++;
	}
	return (i);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	int		i;
	char	*r;
	int		b;
	int		e;
	int		len;

	i = 0;
	b = begin(s1, set);
	e = end(s1, set);
	len = ft_strlen(s1) - b - e;
	if (len < 0)
		len = 0;
	r = malloc(len + 1);
	if (!r)
		return (NULL);
	r[len] = '\0';
	while (i < len)
	{
		r[i] = s1[i + b];
		i++;
	}
	return (r);
}
