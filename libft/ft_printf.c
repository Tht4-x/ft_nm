/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dancel <dancel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 00:00:00 by dancel            #+#    #+#             */
/*   Updated: 2026/09/04 00:00:00 by dancel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	dput_char(char c, int fd)
{
	return (write(fd, &c, 1));
}

static int	dput_str(char *str, int fd)
{
	int	len;
	int	i;

	i = 0;
	len = 0;
	if (!str)
		str = "(null)";
	while (str[i])
		i++;
	len += write(fd, str, i);
	return (len);
}

static int	dput_hex(unsigned long long num, char format, int fd)
{
	char	*hex;
	int		len;

	len = 0;
	if (format == 'x')
		hex = "0123456789abcdef";
	else
		hex = "0123456789ABCDEF";
	if (num >= 16)
		len += dput_hex(num / 16, format, fd);
	len += write(fd, &hex[num % 16], 1);
	return (len);
}

static int	dput_ptr(void *ptr, int fd)
{
	unsigned long long	ull;

	ull = (unsigned long long)ptr;
	if (!ull)
		return (write(fd, "(nil)", 5));
	write(fd, "0x", 2);
	return (dput_hex(ull, 'x', fd) + 2);
}

static int	dput_nbr(int n, int fd)
{
	char	c;
	int		len;
	long	num;

	len = 0;
	num = n;
	if (num < 0)
	{
		len += write(fd, "-", 1);
		num = -num;
	}
	if (num >= 10)
		len += dput_nbr(num / 10, fd);
	c = (num % 10) + '0';
	len += write(fd, &c, 1);
	return (len);
}

static int	dput_nbr_unsigned(unsigned int n, int fd)
{
	char	c;
	int		len;

	len = 0;
	if (n >= 10)
		len += dput_nbr_unsigned(n / 10, fd);
	c = (n % 10) + '0';
	len += write(fd, &c, 1);
	return (len);
}

static int	dput_size(size_t n, int fd)
{
	char	c;
	int		len;

	len = 0;
	if (n >= 10)
		len += dput_size(n / 10, fd);
	c = (n % 10) + '0';
	len += write(fd, &c, 1);
	return (len);
}

static int	dput_ulong(unsigned long n, int fd)
{
	char	c;
	int		len;

	len = 0;
	if (n >= 10)
		len += dput_ulong(n / 10, fd);
	c = (n % 10) + '0';
	len += write(fd, &c, 1);
	return (len);
}

static int	ft_printf_format(const char *format, int i, va_list args,
		int fd, int *extra)
{
	int	len;

	len = 0;
	*extra = 0;
	if (format[i] == 'z' && format[i + 1] == 'u')
	{
		len += dput_size(va_arg(args, size_t), fd);
		*extra = 1;
	}
	else if (format[i] == 'l' && format[i + 1] == 'u')
	{
		len += dput_ulong(va_arg(args, unsigned long), fd);
		*extra = 1;
	}
	else if (format[i] == 'c')
		len += dput_char(va_arg(args, int), fd);
	else if (format[i] == 's')
		len += dput_str(va_arg(args, char *), fd);
	else if (format[i] == 'p')
		len += dput_ptr(va_arg(args, void *), fd);
	else if (format[i] == 'd' || format[i] == 'i')
		len += dput_nbr(va_arg(args, int), fd);
	else if (format[i] == 'u')
		len += dput_nbr_unsigned(va_arg(args, unsigned int), fd);
	else if (format[i] == 'x' || format[i] == 'X')
		len += dput_hex(va_arg(args, unsigned int), format[i], fd);
	else if (format[i] == '%')
		len += dput_char('%', fd);
	else
	{
		len += dput_char('%', fd);
		len += dput_char(format[i], fd);
	}
	return (len);
}

static int	ft_vdprintf(int fd, const char *format, va_list args)
{
	int	len;
	int	i;
	int	extra;

	len = 0;
	i = 0;
	if (format == NULL)
		return (-1);
	while (format[i])
	{
		if (format[i] == '%' && format[i + 1])
		{
			i++;
			len += ft_printf_format(format, i, args, fd, &extra);
			i += extra;
		}
		else if (format[i] == '%' && format[i + 1] == '\0')
			len = -1;
		else
			len += dput_char(format[i], fd);
		i++;
	}
	return (len);
}

int	ft_dprintf(int fd, const char *format, ...)
{
	va_list	args;
	int		len;

	va_start(args, format);
	len = ft_vdprintf(fd, format, args);
	va_end(args);
	return (len);
}

int	ft_printf(const char *format, ...)
{
	va_list	args;
	int		len;

	va_start(args, format);
	len = ft_vdprintf(1, format, args);
	va_end(args);
	return (len);
}
