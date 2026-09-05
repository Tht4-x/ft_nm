/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strprefix.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajeon <chajeon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 15:03:51 by dancel            #+#    #+#             */
/*   Updated: 2025/02/14 17:10:03 by chajeon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strprefix(const char *s, const char *prefix)
{
	if (!s || !*s || !prefix || !*prefix || ft_strlen(s) < ft_strlen(prefix))
		return (0);
	return (ft_strncmp(s, prefix, ft_strlen(prefix)) == 0);
}
