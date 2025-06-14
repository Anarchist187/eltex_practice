#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <net/net_namespace.h>
#include <linux/string.h>

#define NETLINK_USER 31
#define MESSAGE_LEN 256

struct sock *nl_sk = NULL;

static void hello_nl_recv_msg(struct sk_buff *skb);

struct netlink_kernel_cfg cfg = {
   .groups  = 1,
   .input = hello_nl_recv_msg,
};

static void hello_nl_recv_msg(struct sk_buff *skb)
{

    struct nlmsghdr *nlh;
    int pid;
    struct sk_buff *skb_out;
    int msg_size;
    char *msg;
    int res;

    nlh = (struct nlmsghdr *)skb->data;
    printk(KERN_INFO "Netlink received msg payload: %s\n", (char *)nlmsg_data(nlh));
    pid = nlh->nlmsg_pid; /*pid of sending process */

    msg=kmalloc(MESSAGE_LEN*sizeof(char),GFP_KERNEL);
    msg=strcat(msg, "Message: ");
    msg=strcat(msg, nlmsg_data(nlh));
    msg_size=strlen(msg);

    skb_out = nlmsg_new(msg_size, 0);

    if (!skb_out)
    {

        printk(KERN_ERR "Failed to allocate new skb\n");
        return;

    }
    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
    strncpy(nlmsg_data(nlh), msg, msg_size);

    res = nlmsg_unicast(nl_sk, skb_out, pid);

    if (res < 0)
        printk(KERN_INFO "Error while sending bak to user\n");
}


static int __init mod_init(void)
{

    printk("Entering: %s\n", __FUNCTION__);
    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);

    if (!nl_sk)
    {
        printk(KERN_ALERT "Error creating socket.\n");
        return -10;
    }

    return 0;
}

static void __exit mod_exit(void)
{

    printk(KERN_INFO "exiting module\n");
    netlink_kernel_release(nl_sk);
}
module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sablin M.A.");
MODULE_DESCRIPTION("Module for message exchange via netlink");