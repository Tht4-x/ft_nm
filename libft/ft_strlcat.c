/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajeon <chajeon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 16:20:53 by dancel            #+#    #+#             */
/*   Updated: 2025/02/14 17:10:29 by chajeon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dest, const char *src, size_t size)
{
	size_t			ldest;
	size_t			lsrc;
	unsigned int	i;

	i = 0;
	ldest = 0;
	lsrc = 0;
	while (ldest < size && dest[ldest])
		ldest++;
	while (src[lsrc])
		lsrc++;
	if (size == 0)
		return (lsrc);
	if (ldest >= size)
		return (size + lsrc);
	while (src[i] && (ldest + i) < (size - 1))
	{
		dest[ldest + i] = src[i];
		i++;
	}
	dest[ldest + i] = '\0';
	return (ldest + lsrc);
}
