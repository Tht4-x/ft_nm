/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajeon <chajeon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 21:16:27 by dancel            #+#    #+#             */
/*   Updated: 2025/02/14 17:11:37 by chajeon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstchr(t_list *lst, void *s)
{
	if (!s)
		return (NULL);
	while (lst)
	{
		if (ft_strncmp(lst->content, s, ft_strlen(s)))
			lst = lst->next;
		else
			return (lst);
	}
	return (NULL);
}
