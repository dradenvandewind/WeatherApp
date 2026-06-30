# Terraform + Ansible Deployment — weather-app

## Architecture
- **Terraform** provisions an Ubuntu 22.04 EC2 instance on AWS (Paris region by default), a security group opening ports 22 (SSH), 8080 (Nginx), 3000 (Grafana), 9090 (Prometheus), and an SSH key pair.
- **Ansible** installs Docker + the compose plugin on the VM, copies your project, then runs `docker compose up -d --build`.

## Prerequisites
- Terraform >= 1.5
- Ansible >= 2.14 (with the `synchronize` module, which requires `rsync` installed locally and on the target)
- Valid AWS credentials (`aws configure` or `AWS_ACCESS_KEY_ID` / `AWS_SECRET_ACCESS_KEY` environment variables)

## Step 1 — Application code now comes directly from GitHub
Ansible clones (or updates via `git pull`) the repository **https://github.com/dradenvandewind/WeatherApp.git** directly on the VM under `/opt/weather-app`. You no longer need to manually place files in `deploy/ansible/files/weather-app/` (that folder is no longer used; you can keep it or remove it).

## Step 2 — Provision the infrastructure
```bash
cd terraform
terraform init
terraform plan
terraform apply
```
This creates the VM and **automatically generates** `../ansible/inventory.ini` with the correct IP and SSH key (`weather-app-key.pem`).

## Step 3 — Deploy the application
```bash
cd ../ansible
ansible-playbook playbook.yml
```

## Step 4 — Access the services
- App (via Nginx): `http://<PUBLIC_IP>:8080`
- Grafana: `http://<PUBLIC_IP>:3000` (admin/admin)
- Prometheus: `http://<PUBLIC_IP>:9090`

The public IP is shown in the output of `terraform apply` (output `public_ip`).

## To destroy the infrastructure
```bash
cd terraform
terraform destroy
```

## Notes / items to adapt
- `var.ssh_allowed_cidr` is open to `0.0.0.0/0` by default: restrict it to your IP in production.
- `t3.medium` (4 GB RAM) is a comfortable minimum given the number of containers (Drogon, Nginx, Prometheus, Grafana, node-exporter, cAdvisor); adjust `var.instance_type` according to your workload.

