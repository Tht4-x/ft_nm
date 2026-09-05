/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajeon <chajeon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 14:14:50 by dancel            #+#    #+#             */
/*   Updated: 2025/02/14 17:09:51 by chajeon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char			*copy;
	unsigned int	i;

	i = 0;
	if (start >= ft_strlen(s))
	{
		copy = malloc(1);
		if (!copy)
			return (NULL);
		copy[0] = '\0';
		return (copy);
	}
	if (len > ft_strlen(s) - start)
		len = ft_strlen(s) - start;
	copy = malloc(len + 1);
	if (copy == NULL)
		return (NULL);
	while (i < (unsigned int)len)
	{
		copy[i] = s[i + start];
		i++;
	}
	copy[i] = '\0';
	return (copy);
}
