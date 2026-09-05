/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strsuffix.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajeon <chajeon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/03 19:22:02 by dancel            #+#    #+#             */
/*   Updated: 2025/02/14 17:09:56 by chajeon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strsuffix(const char *s, const char *suffix)
{
	if (!s || !*s || !suffix || !*suffix || ft_strlen(s) < ft_strlen(suffix))
		return (0);
	return (ft_strcmp(s + ft_strlen(s) - ft_strlen(suffix), suffix) == 0);
}
