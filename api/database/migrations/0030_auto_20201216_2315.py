# Generated by Django 3.1.4 on 2020-12-16 23:15

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('database', '0029_auto_20201216_2312'),
    ]

    operations = [
        migrations.AlterField(
            model_name='contest',
            name='problems',
            field=models.ManyToManyField(blank=True, to='database.Problem'),
        ),
    ]
