/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajeon <chajeon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 12:46:39 by dancel            #+#    #+#             */
/*   Updated: 2025/02/14 17:11:45 by chajeon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_strlen_itoa(int n)
{
	int	ft_strlen_itoa;

	ft_strlen_itoa = 0;
	if (n <= 0)
		ft_strlen_itoa++;
	while (n != 0)
	{
		n = n / 10;
		ft_strlen_itoa++;
	}
	return (ft_strlen_itoa);
}

char	*ft_itoa(int n)
{
	long	m;
	int		l;
	char	*r;

	m = n;
	l = ft_strlen_itoa(n);
	r = malloc(sizeof(char) * (l + 1));
	if (r == NULL)
		return (NULL);
	if (m < 0)
	{
		r[0] = '-';
		m = -m;
	}
	r[l] = '\0';
	if (m == 0)
		r[0] = '0';
	while (m > 0)
	{
		r[--l] = m % 10 + 48;
		m = m / 10;
	}
	return (r);
}
